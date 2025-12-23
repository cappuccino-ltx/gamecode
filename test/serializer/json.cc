
#include <jsoncpp/json/json.h>
#include <memory>
#include "log.h"

int main() {
    Json::Value root1;
    root1["name"] = "zhangsan";
    root1["age"] = 10;
    std::unique_ptr<Json::StreamWriter>writer(Json::StreamWriterBuilder().newStreamWriter());
    std::stringstream out;
    writer->write(root1, &out);
    infolog << out.str();

    std::unique_ptr<Json::CharReader> reader(Json::CharReaderBuilder().newCharReader());
    Json::Value root2;
    std::string in(out.str());
    reader->parse(in.data(), in.data() + in.size(), &root2, nullptr);
    infolog("name {}, age {}",root2["name"].asString(),root2["age"].asInt());

    return 0;
}