#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

// 服务器异常退出处理函数
void resetHandler(int) {
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv) {

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", atoi(argv[1]));
    ChatServer server(&loop, addr, "ChatServer");
    
    server.start();
    loop.loop();

    return 0;
}