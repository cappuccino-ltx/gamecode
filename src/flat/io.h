#pragma once 
#include "command_fb.h"
#include "frame_fb.h"
#include "entity_fb.h"
#include "enet.h"
#include "entity.h"
#include <flatbuffers/flatbuffers.h>

namespace io{

class Command{
public:
    Command(const flat::Command* comm = nullptr)
        :command(comm)
    {}
    // 反序列化
    bool deserializer(std::shared_ptr<enet::ENetData>& pack){
        if (!flatbuffers::BufferHasIdentifier(pack->data.data(), flat::CommandIdentifier())){
            errorlog << "identifier failed";
            return false;
        }
        packet = pack;
        command = flat::GetCommand(packet->data.data());
        return true;
    }
    const flat::Command* operator->() const {
        return command;
    }
    // 序列化
    static std::vector<uint8_t> serializer(
        uint32_t player_id = 0,
        uint32_t room_id = 0,
        int8_t move_x = 0,
        int8_t move_y = 0,
        bool attack = false,
        bool skill1 = false,
        bool skill2 = false
    ){
        flatbuffers::FlatBufferBuilder builder(256);
        auto command_offset = flat::CreateCommand(
            builder,player_id,room_id,move_x,move_y,attack,skill1,skill2
        );
        builder.Finish(command_offset,flat::CommandIdentifier());
        uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        return std::vector<uint8_t>{buf, buf + size};
    }

private:
    const flat::Command* command = nullptr;
    std::shared_ptr<enet::ENetData> packet;
};
class Entity{
public:
    Entity(const flat::Entity* enti = nullptr)
        :entity(enti)
    {}
    // 反序列化
    bool deserializer(std::shared_ptr<enet::ENetData>& pack){
        if (!flatbuffers::BufferHasIdentifier(pack->data.data(), flat::CommandIdentifier())){
            errorlog << "identifier failed";
            return false;
        }
        packet = pack;
        entity = flat::GetEntity(packet->data.data());
        return true;
    }
    const flat::Entity* operator->(){
        return entity;
    }
    // 序列化
    static std::vector<uint8_t> serializer(
        uint32_t entity_id = 0,
        int32_t hp = 0,
        uint32_t status_flags = 0,
        float x = 0.0f,
        float y = 0.0f,
        float vx = 0.0f,
        float vy = 0.0f,
        float attack_cooldown = 0.0f,
        float skill1_cooldown = 0.0f,
        float skill2_colldown = 0.0f
    ){
        flatbuffers::FlatBufferBuilder builder(256);
        auto command_offset = flat::CreateEntity(
            builder,entity_id,hp,status_flags,x,y,vx,vy
            ,attack_cooldown,skill1_cooldown,skill2_colldown
        );
        builder.Finish(command_offset,flat::EntityIdentifier());
        uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        return std::vector<uint8_t>{buf, buf + size};
    }
    static std::vector<uint8_t> serializer(entity::Entity& eneity){
        flatbuffers::FlatBufferBuilder builder(256);
        auto command_offset = flat::CreateEntity(
            builder,eneity.entity_id,eneity.hp,eneity.status_flags,eneity.x,eneity.y,
            eneity.vx,eneity.vy,eneity.attack_cooldown,eneity.skill1_cooldown,eneity.skill2_colldown
        );
        builder.Finish(command_offset,flat::EntityIdentifier());
        uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        return std::vector<uint8_t>{buf, buf + size};
    }
    


private:
    const flat::Entity* entity = nullptr;
    std::shared_ptr<enet::ENetData> packet;
};
class Frame{
public:
    // 反序列化
    bool deserializer(std::shared_ptr<enet::ENetData>& packet){
        if (!flatbuffers::BufferHasIdentifier(packet->data.data(), flat::FrameIdentifier())){
            errorlog << "identifier failed";
            return false;
        }
        frame = flat::GetFrame(packet->data.data());
        return true;
    }
    uint32_t tick(){
        return frame->tick();
    }
    flat::DataType type(){
        return frame->type();
    }

    auto command_begin(){
        return frame->commands()->begin();
    }
    auto command_end(){
        return frame->commands()->end();
    }
    size_t command_size(){
        return frame->commands()->size();
    }
    Command get_command(int i){
        return frame->commands()->Get(i);
    }

    auto entities_begin(){
        return frame->entities()->begin();
    }
    auto entities_end(){
        return frame->entities()->end();
    }
    size_t entities_size(){
        return frame->entities()->size();
    }
    Entity get_entities(int i){
        return frame->entities()->Get(i);
    }

    // 序列化
    static std::vector<uint8_t> serializer(
        uint32_t tick_,
        flat::DataType type_,
        const std::vector<io::Command>& commands,
        const std::vector<entity::Entity>& entities
    ){
        flatbuffers::FlatBufferBuilder builder(4096);
        std::vector<flatbuffers::Offset<flat::Command>> cv(commands.size());
        std::vector<flatbuffers::Offset<flat::Entity>> ev(entities.size());
        flatbuffers::Offset<flat::Frame> frame_offset = 0;
        if (type_ == flat::DataType::DataType_Entitys){
            for (int i = 0; i < entities.size(); ++i) {
                ev[i] = flat::CreateEntity(builder,
                    entities[i].entity_id,
                    entities[i].hp,
                    entities[i].status_flags,
                    entities[i].x,
                    entities[i].y,
                    entities[i].vx,
                    entities[i].vy,
                    entities[i].attack_cooldown,
                    entities[i].skill1_cooldown,
                    entities[i].skill2_colldown
                );
            }
            auto ev_offset = builder.CreateVector(ev);
            frame_offset = flat::CreateFrame(builder,tick_,type_,ev_offset,0);
        }else if (type_ == flat::DataType::DataType_Commands) {
            for (int i = 0; i < commands.size(); ++i) {
                cv[i] = flat::CreateCommand(builder,
                    commands[i]->player_id(),
                    commands[i]->room_id(),
                    commands[i]->move_x(),
                    commands[i]->move_y(),
                    commands[i]->attack(),
                    commands[i]->skill1(),
                    commands[i]->skill2()
                );
            }
            auto cv_offset = builder.CreateVector(cv);
            frame_offset = flat::CreateFrame(builder,tick_,type_,0,cv_offset);
        }
        
        builder.Finish(frame_offset,flat::FrameIdentifier());
        uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        return std::vector<uint8_t>{buf, buf + size};
    }

private:
    const flat::Frame* frame = nullptr;
};

}