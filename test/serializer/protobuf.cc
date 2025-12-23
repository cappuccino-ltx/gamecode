
#include <log.h>
#include <string>
#include "test.pb.h"

int main() {
    test::User u1;
    u1.set_name("张三");
    u1.set_age(12);

    std::string str;
    u1.SerializeToString(&str);
    infolog << str;

    test::User u2;
    u2.ParseFromString(str);
    infolog("name : {}, age : {}",u2.name(),u2.age());

    return 0;
}

// g++ -o proto protobuf.cc test.pb.cc -lprotobuf