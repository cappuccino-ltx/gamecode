#pragma once 

#include "lfree.h"
#include "log.h"
#include "enet.h"
#include <cstdint>
#include <unordered_map>

namespace conn{

    // 管理客户端的链接,
class Connect{
    using UserId = uint32_t;
    using ConnId = uint32_t;
public:
    Connect(enet::ENetServer& s);

    void start();

private:
    // 回调函数,由enet层线程调用
    void disConnBack(ConnId id);
    // 处理enet层压入的用户断开的消息,
    void handleDisconn();

private:
    enet::ENetServer& server;
    std::unordered_map<UserId, ConnId> uc_map;
    std::unordered_map<ConnId, UserId> cu_map;
    lfree::ring_queue<ConnId> disconn {lfree::queue_size::K01};
};

}// unpack