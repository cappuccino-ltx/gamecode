#pragma once 

#include "log.h"
#include "enet.h"
#include <thread>


namespace server{
class Server{
public:
    Server(uint16_t port,uint32_t client_limit = 256,uint32_t channel_n = 1)
        :net(port,client_limit,channel_n)
        ,net_thread(&Server::net_handler, this)
    {

    }
    
    void start(){
        
    }

private:
    // 网络层线程处理函数
    void net_handler(){
        // 负责收发网络消息
        net.start(0);
    }

private:
    enet::ENetServer net;
    std::thread net_thread;
}; // class Server
}// namespace server