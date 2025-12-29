#pragma once 

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <log.h>
#include <lfree.h>
#include <enet/enet.h>
#include <memory>
#include <unordered_map>

namespace enet{

struct ENetData{
    ENetData(){};
    ENetData(uint32_t sid,const std::string& pack,uint32_t cid)
        :session_id(0),data(pack.data(),pack.data() + pack.size()),channel_id(cid)
    {}
    ENetData(const std::string& pack,uint32_t cid)
        :session_id(0),data(pack.data(),pack.data() + pack.size()),channel_id(cid)
    {}
    ENetData(uint32_t sid,const std::vector<uint8_t>& pack,uint32_t cid)
        :session_id(0),data(pack.data(),pack.data() + pack.size()),channel_id(cid)
    {}
    ENetData(const std::vector<uint8_t>& pack,uint32_t cid)
        :session_id(0),data(pack.data(),pack.data() + pack.size()),channel_id(cid)
    {}
    ENetData(uint32_t sid, std::vector<uint8_t>&& pack,uint32_t cid)
        :session_id(0),data(std::move(pack)),channel_id(cid)
    {}
    ENetData(std::vector<uint8_t>&& pack,uint32_t cid)
        :session_id(0),data(std::move(pack)),channel_id(cid)
    {}
    ENetData(uint32_t sid,ENetPacket* pack,uint32_t cid)
        :session_id(sid),data(pack->data,pack->data + pack->dataLength),channel_id(cid)
    {}
    static std::shared_ptr<ENetData> make_data(uint32_t sid,const std::string& pack,uint32_t cid){
        return std::make_shared<ENetData>(sid,pack,cid);
    }
    static std::shared_ptr<ENetData> make_data(const std::string& pack,uint32_t cid){
        return std::make_shared<ENetData>(pack,cid);
    }
    static std::shared_ptr<ENetData> make_data(uint32_t sid,const std::vector<uint8_t>& pack,uint32_t cid){
        return std::make_shared<ENetData>(sid,pack,cid);
    }
    static std::shared_ptr<ENetData> make_data(const std::vector<uint8_t>& pack,uint32_t cid){
        return std::make_shared<ENetData>(pack,cid);
    }
    static std::shared_ptr<ENetData> make_data(uint32_t sid, std::vector<uint8_t>&& pack,uint32_t cid){
        return std::make_shared<ENetData>(sid,std::move(pack),cid);
    }
    static std::shared_ptr<ENetData> make_data(std::vector<uint8_t>&& pack,uint32_t cid){
        return std::make_shared<ENetData>(std::move(pack),cid);
    }
    uint32_t session_id;
    uint32_t channel_id;
    std::vector<uint8_t> data;
};


class Init{
    friend class ENetServer;
    friend class ENetClient;

    static void getInit(){
        static Init init;
    }
    Init(){
        if (0 != enet_initialize()){
            errorlog << "failed to initialize enet";
            exit(1);
        }
    }
    ~Init(){
        enet_deinitialize();
    }
};

class ENetServer{
public:
    ENetServer(uint16_t port,uint32_t client_limit = 256,uint32_t channel_n = 1,uint32_t in_limit = 0,uint32_t out_limit = 0)
    {
        Init::getInit();
        ENetAddress addr;
        enet_address_set_host(&addr, "0.0.0.0");
        addr.port = port;
        server = enet_host_create(&addr, client_limit, channel_n, in_limit, out_limit);
        if (server == nullptr) {
            errorlog << "failed to create enet server";
            exit(2);
        }
    }

    void start(uint32_t timeout = 0){
        ENetEvent event;
        while(running.load() && enet_host_service(server, &event, timeout) >= 0) {
            switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:{
                onConnect(&event);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:{
                std::shared_ptr<ENetData> data = std::make_shared<ENetData>(ids[event.peer],event.packet,event.channelID);
                receives.put(data);
                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:{
                // 连接断开
                onDisConnect(&event);
                break;
            }
            case ENET_EVENT_TYPE_NONE:{
                // ...
                break;
            }
            }
            // 处理发送任务
            onSend();
            onDisConnect();
        }
    }

    void quit(){
        running.store(false,std::memory_order_release);
    }

    bool read(std::shared_ptr<ENetData>* data){
        return receives.get(*data);
    }

    void send(const std::shared_ptr<ENetData>& data){
        sends.put(data);
    }

    void disconnect(size_t sid){
        disconnectTask.put(sid);
    }

    ~ENetServer(){
        if (server){
            enet_host_destroy(server);
            server = nullptr;
        }
    }

private:
    uint32_t getSession(){
        while (1) {
            if (peers.count(++sessionid) != 0) {
                continue;
            }
            if (sessionid == 0) {
                continue;
            }
            return sessionid;
        }
        return 0;
    }

    void onConnect(ENetEvent* event){
        // 获取连接, 分配新的sessionid
        uint32_t session = getSession();
        peers[session] = event->peer;
        ids[event->peer] = session;
    }
    void onDisConnect(ENetEvent* event){
        auto id = ids.find(event->peer);
        if (id == ids.end()) {
            return ;
        }
        auto pe = peers.find(id->second);
        if (pe == peers.end()){
            ids.erase(id);
            return ;
        }
        peers.erase(pe);
        ids.erase(id);
    }
    void onDisConnect(){
        while(disconnectTask.readable()) {
            size_t sid;
            bool ret = disconnectTask.try_get(sid);
            if (ret == false){
                return ;
            }
            auto pe = peers.find(sid);
            if (pe == peers.end()){
                continue ;
            }
            enet_peer_disconnect(pe->second, 1); // 断开连接
            auto id = ids.find(pe->second);
            if (id == ids.end()) {
                peers.erase(pe);
                continue ;
            }
            peers.erase(pe);
            ids.erase(id);
        }
    }

    void onSend(){
        while(sends.readable()){
            std::shared_ptr<ENetData> task;
            bool ret = sends.try_get(task);
            if (ret == false) {
                return ;
            }
            ENetPacket * packet = enet_packet_create(task->data.data(), task->data.size(), ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_NO_ALLOCATE);
            // 如果使用了no allocate 的话,要保证这个task不能释放,需要再packetfreecallback中释放
            packet->userData = new std::shared_ptr<ENetData>(task);
            packet->freeCallback = packetFreeCallback;
            
            auto it = peers.find(task->session_id);
            if (it != peers.end()){
                enet_peer_send(it->second, task->channel_id, packet);
            }else {
                // 这里也会调用自定义的释放函数去释放,
                enet_packet_destroy(packet);
            }
        }
    }

static void packetFreeCallback(ENetPacket* packet){
    auto data = static_cast<std::shared_ptr<ENetData>*>(packet->userData);
    delete data;
}

private:
    ENetHost* server = nullptr;
    size_t sessionid = 0 ;
    std::atomic<bool> running { true };
    std::unordered_map<uint32_t, ENetPeer*> peers;
    std::unordered_map<ENetPeer*,uint32_t> ids;
    lfree::ring_queue<std::shared_ptr<ENetData>> receives{lfree::queue_size::K2};
    lfree::ring_queue<std::shared_ptr<ENetData>> sends{lfree::queue_size::K2};
    lfree::ring_queue<size_t> disconnectTask{lfree::queue_size::K003};
}; // class ENetServer
enum Status{
    NotStarted,
    Connecting,
    Connected,
    Disconnected,
    Error
} ;
class ENetClient{
public:
    ENetClient(const std::string& i, uint16_t p, int channle_n = 1,int timeout = 0)
        :ip(i),port(p)
        ,channel_num(channle_n)
        ,thread_client(std::bind(&ENetClient::handler,this,timeout))
    {
        Init::getInit();
    }

    ~ENetClient(){
        quit();
        thread_client.join();
    }

    bool read(std::shared_ptr<ENetData>* data){
        return receives.get(*data);
    }

    bool send(const std::shared_ptr<ENetData>& data){
        if (status == Connected){
            sends.put(data);
            return true;
        }
        return false;
    }

    void quit(){
        running.store(false,std::memory_order_release);
        receives.quit();
    }

    Status statu(){
        return status;
    }

private:
    void handler(int timeout){
        init();
        start(timeout);
    }
    void init(){
        client = enet_host_create(nullptr, 1, channel_num, 0, 0);
        if (client == nullptr) {
            errorlog << "failed to craete host";
            exit(1);
        }
    }
    void start(int timeout){
        ENetAddress ser_host;
        enet_address_set_host(&ser_host, ip.data());
        ser_host.port = port;
        while(running.load(std::memory_order_acquire)){
            // 连接客户端
            server_peer = enet_host_connect(client,&ser_host,channel_num,0);
            if (server_peer == nullptr) {
                errorlog << "server peer is nullptr ";
            }
            status = Connecting;
            // 通信
            ENetEvent event;
            while(running.load(std::memory_order_relaxed) && enet_host_service(client, &event, timeout) >= 0){
                switch(event.type) {
                case ENET_EVENT_TYPE_CONNECT:{
                    status = Connected;
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE:{
                    std::shared_ptr<ENetData> data = std::make_shared<ENetData>(0,event.packet,event.channelID);
                    receives.put(data);
                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:{
                    if (event.data == 1){
                        // 服务器断开连接,
                        status = Disconnected;
                        running.store(false,std::memory_order_relaxed);
                    }else {
                        // 其他情况断开连接需要重连
                        status = Error;
                    }
                    enet_peer_reset(server_peer);
                    server_peer = nullptr;
                    break;
                }
                case ENET_EVENT_TYPE_NONE:{
                    // ...
                    break;
                }
                }
                if (status == Error){
                    break;
                }
                // 发送任务执行
                onSend();
            }
            if(status == Error || status == Connecting){
                // 每3秒重连一次
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }
        // 在退出之前应该要做一些清理工作,
        // 将任务队列中的任务读取完
        while(receives.readable()){
            std::shared_ptr<ENetData> data;
            receives.get(data);
        }
        while(sends.readable()){
            std::shared_ptr<ENetData> data;
            sends.get(data);
        }
        // 析构在线程内完成
        enet_host_destroy(client);
        return;
    }

    void onSend(){
        while (sends.readable()){
            std::shared_ptr<ENetData> task;
            bool ret = sends.try_get(task);
            if (ret == false){
                return ;
            }
            ENetPacket* packet = enet_packet_create(task->data.data(), task->data.size(), ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_NO_ALLOCATE);
            packet->userData = new std::shared_ptr<ENetData>(task);
            packet->freeCallback = packetFreeCallback;
            if (server_peer){
                enet_peer_send(server_peer, task->channel_id, packet);
            }
            else {
                enet_packet_destroy(packet);
            }
        }
    }
    static void packetFreeCallback(ENetPacket* packet){
        auto data = static_cast<std::shared_ptr<ENetData>*>(packet->userData);
        delete data;
    }
    
private:
    ENetHost* client = nullptr;
    ENetPeer* server_peer = nullptr;
    std::string ip;
    uint16_t port;
    uint16_t channel_num;
    std::atomic<bool> running { true };
    std::atomic<Status> status { NotStarted };
    lfree::ring_queue<std::shared_ptr<ENetData>> receives{lfree::queue_size::K2};
    lfree::ring_queue<std::shared_ptr<ENetData>> sends{lfree::queue_size::K2};
    std::thread thread_client;
};
} //  namespace enet