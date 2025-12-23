


#include <cstdint>
#include <flatbuffers/flatbuffers.h>
#include <log.h>
#include <sys/types.h>
#include <vector>
#include "shared_generated.h"
#include "request_generated.h"

std::vector<uint8_t> serializer(uint64_t id, test::Status status, const std::string& message,const std::vector<std::string>& names) {
    flatbuffers::FlatBufferBuilder builder(1024); // 给这个builder 1024的空间大小

    // 序列化子对象
    auto message_offset = builder.CreateString(message);
    std::vector<flatbuffers::Offset<flatbuffers::String>> names_temp(names.size());
    for (int i = 0; i < names.size(); ++i) {
        names_temp[i] = builder.CreateString(names[i]);
    }
    auto names_offset = builder.CreateVector(names_temp);

    // 构建request对象
    auto request_offset = test::Createrequest(builder,id,status,message_offset,names_offset);

    // 完成对象的序列化, 使用生成的类型标识符
    builder.Finish(request_offset,test::requestIdentifier());

    // 将缓冲区的数据拷贝出来,并返回
    uint8_t* buf = builder.GetBufferPointer();
    size_t size = builder.GetSize();
    return std::vector<uint8_t>{buf, buf + size};
}

void deserializer(std::vector<uint8_t>& data){
    if (!flatbuffers::BufferHasIdentifier(data.data(), test::requestIdentifier())){
        errorlog << "identifier failed";
        return;
    }
    auto request = test::Getrequest(data.data());
    infolog("id : {}", request->req_id());
    infolog("status : {}", (int)request->status());
    infolog("message : {}", request->message()->c_str());
    auto names = request->names();
    for (int i = 0; i < names->size(); i++) {
        infolog << names->Get(i)->c_str();
    }
}


int main() {

    std::vector<uint8_t> data = serializer(100, test::Status_Success, "hello", {"zhangsan","lisi"});
    deserializer(data);

    return 0;
}

// 编译运行
// g++ -o flatbuffers flatbuffers.cc -lflatbuffers

// output
/*
[2025-12-11 14:28:11] [INFO] [flatbuffers.cc:41] id : 100
[2025-12-11 14:28:11] [INFO] [flatbuffers.cc:42] status : 0
[2025-12-11 14:28:11] [INFO] [flatbuffers.cc:43] message : hello
[2025-12-11 14:28:11] [INFO] [flatbuffers.cc:46] zhangsan
[2025-12-11 14:28:11] [INFO] [flatbuffers.cc:46] lisi
*/