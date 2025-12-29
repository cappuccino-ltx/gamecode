
#include "../../src/comm/enet.h"
#include <string>
#include <thread>

int main() {
    enet::ENetClient client("127.0.0.1",8080);
    std::thread t([&client](){
        while(1){
            std::shared_ptr<enet::ENetData> data;
            bool ret = client.read(&data);
            if (!ret) break;
            infolog << std::string{data->data.begin(),data->data.end()};
        }
    });

    while(1){
        std::string input;
        std::getline(std::cin,input);
        if (input == "quit"){
            break;
        }
        client.send(enet::ENetData::make_data(input,0));
    }

    client.quit();
    t.join();

    return 0;
}