#pragma once 

#include <cstdint>
namespace entity{

// 在服务端使用不到这个结构
struct Command{
    uint32_t player_id = 0;
    uint32_t room_id = 0;
    int8_t move_x = 0;
    int8_t move_y = 0;
    bool attack = false;
    bool skill1 = false;
    bool skill2 = false;
}; // struct Command

} // namespace entity