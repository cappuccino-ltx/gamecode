

#include "log.h"
#include <entt/entt.hpp>

struct Position{
    float x, y, z;
};

int main() {
    entt::registry registery;
    auto e = registery.create();
    registery.emplace<Position>(e,1.1,2.2,3.3);
    
    return 0;
}