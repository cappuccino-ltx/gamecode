
#include "server.h"

int main() {
    // 允许256个客户端连接,每个链接一个通道
    server::Server svr(8080,256,1);
    svr.start();
    return 0;
}