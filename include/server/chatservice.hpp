#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include "json.hpp"

using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;

// 回调函数类型
using MsgHandler = std::function<void(const TcpConnectionPtr&, json&, Timestamp)>;

// 聊天服务器业务类
class ChatService {
public:
    // ChatService 单例模式  线程安全
    static ChatService* instance() {
        static ChatService service;
        return &service;
    }

    // 登录业务
    void loginHandler(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 注册业务
    void registerHandler(const TcpConnectionPtr& conn, json& js, Timestamp time);

    MsgHandler getHandler(int msgId);

private:
    ChatService();
    ChatService(const ChatService& ) = delete;
    ChatService& operator=(const ChatService&) = delete;
    
    // 存储消息id和其对应的业务处理方法
    std::unordered_map<int, MsgHandler> _msgHandlerMap;
    
};
#endif // !CHATSERVICE_H
