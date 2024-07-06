#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include <vector>

using namespace muduo;
using namespace std;

ChatService::ChatService() {
    // 对各类消息处理方法的注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::loginHandler, this, _1, _2, _3)});
    _msgHandlerMap.insert({RETISTER_MSG, std::bind(&ChatService::registerHandler, this, _1, _2, _3)});
}

void ChatService::loginHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    LOG_INFO << "do login service!";
}

void ChatService::registerHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    LOG_INFO << "do register service!";
}

MsgHandler ChatService::getHandler(int msgId) {
    if (!_msgHandlerMap.count(msgId)) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp) {
            // LOG_ERROR << "msgId: " << msgId << " can not find handler!";
        };
    }
    return _msgHandlerMap[msgId];
}

