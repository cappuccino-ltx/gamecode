

#include "log.h"
#include <entt/entt.hpp>

struct Position{ float x, y; };
struct Velocity { float dx, dy; };

int main() {
    entt::registry registry;
    // 创建实体
    auto e1 = registry.create();
    auto e2 = registry.create();

    // 增
    // 用emplace 或者insert来为实体绑定数据
    registry.emplace<Position>(e1,1.1,2.2);
    registry.emplace<Velocity>(e1,1.1,2.2);
    registry.emplace<Position>(e2,1.1,2.2);
    registry.emplace<Velocity>(e2,1.1,2.2);

    // 查
    // 使用 get 来获取某一个实体的某一个数据
    infolog << registry.get<Position>(e1).x ;
    // 使用view 来获取一个高效的迭代器
    // 获取拥有这两个类型数据的实体
    auto v1 = registry.view<Position, Velocity>();
    for(auto e : v1){
        auto& pos = v1.get<Position>(e);
        auto& vel = v1.get<Velocity>(e);
    }

    // 改
    // 使用 get 或者view 获取到实体的数据引用之后可以直接修改
    registry.get<Position>(e1).x = 10;
    // 使用 patch 可以通知系统该系统已经更改
    registry.patch<Position>(e1, [](auto& pos){
        pos.x += 10.1f;
    });
    // 使用replace 可以完全覆盖
    registry.replace<Position>(e1, 3.3,4.4);

    // 删
    // 通过remove删除对应的数据,保留实体
    registry.remove<Velocity>(e1);
    // 通过destroy 销毁实体,联通实体管理的所有组件
    registry.destroy(e1);
    
    return 0;
}