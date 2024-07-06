#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 聊天服务器初始化
ChatServer::ChatServer(EventLoop* loop, 
                const InetAddress& listenAdder,
                const std::string& nameArg) 
    : _server(loop, listenAdder, nameArg) {
    
    // 注册连接事件的回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息事件的回调函数
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    
}

// 启动服务
void ChatServer::start() {
    _server.start();
}

// 连接事件相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr& conn) {
    // 客户端断开连接
    if (!conn->connected()) {
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr& conn, 
                            Buffer* buffer,
                            Timestamp time) {
    // 将json数据转换为string
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    auto msgHandler = ChatService::instance()->getHandler(js["msgId"].get<int>());

    // 执行对应的业务处理
    msgHandler(conn, js, time);
}

