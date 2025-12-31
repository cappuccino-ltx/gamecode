#pragma once 

#include <cstdint>
namespace entity{


struct Entity{
    uint32_t entity_id = 0;
    int32_t hp = 0;
    uint32_t status_flags = 0;
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float attack_cooldown = 0.0f;
    float skill1_cooldown = 0.0f;
    float skill2_colldown = 0.0f;
}; // struct Command

} // namespace entity