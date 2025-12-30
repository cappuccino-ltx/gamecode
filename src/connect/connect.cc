

#include "connect.h"

// 进行函数的实现
namespace conn{


Connect::Connect(enet::ENetServer& s)
    :server(s)
{}

void Connect::start(){
    while(1){
        // 从server中读取出数据包,反序列化之后,封装,交给world
        std::shared_ptr<enet::ENetData> pack;
        bool ret = server.try_read(&pack);
        if (ret) {
            // 进行反序列化,投递给world层
        }
        // 接收world的数据,封装成广播任务,进行广播
        
    }
}

void Connect::disConnBack(ConnId id){
    disconn.put(id);
}
void Connect::handleDisconn(){
    while(disconn.readable()) {
        ConnId cid = 0;
        if (disconn.try_get(cid)){
            // 读取到了断开连接的客户端,
            // 删除对应的用户id和连接id之间的映射
            auto cu_it = cu_map.find(cid);
            if (cu_it == cu_map.end()){
                continue;
            }
            auto uc_it = uc_map.find(cu_it->second);
            if (uc_it == uc_map.end()){
                cu_map.erase(cu_it);
                continue;
            }
            cu_map.erase(cu_it);
            // 这里并不会删除用户id,用以用户重连的时候,找到对应的用户实体
            uc_it->second = 0;
        }
    }
}

}// unpack