#pragma once 

#include "log.h"
#include "enet.h"

namespace conn{

class Connect{
public:
    Connect(enet::ENetServer& s)
        :server(s)
    {}

    void start(){
        // 从server中读取出数据包,反序列化之后,封装,交给world
        // 接收world的数据,封装成广播任务,进行广播
    }

private:
    enet::ENetServer& server;
};

}// unpack