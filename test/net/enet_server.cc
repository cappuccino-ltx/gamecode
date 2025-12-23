
#include "../../comm/enet.h"
#include <cstdint>
#include <string>

int main() {
    enet::ENetServer server(8080,256);

    std::thread t{[&](){
        while(1){
            std::shared_ptr<enet::ENetData> data;
            bool ret = server.read(&data);
            if (!ret) {
                break;
            }
            std::string str(data->data.begin(),data->data.end());
            str = "client [" + std::to_string(data->session_id) + "] : " + str;
            infolog << str;
            data->data.clear();
            data->data = std::vector<uint8_t>(str.begin(),str.end());
            server.send(data);
        }
    }};
    server.start(0);
    return 0;
}