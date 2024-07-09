#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"

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
    
    // 服务器异常退出处理
    void reset(); 

    // 客户端异常退出处理
    void clientCloseExceptionHandler(const TcpConnectionPtr& conn);

    // 一对一聊天处理
    void oneChatHandler(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 添加好友处理
    void addFriendHandler(const TcpConnectionPtr& conn, json& js, Timestamp time);
private:
    ChatService();
    ChatService(const ChatService& ) = delete;
    ChatService& operator=(const ChatService&) = delete;
    
    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    // 互斥锁
    mutex _connMutex;  
    // 用户id和对应的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
};
#endif // !CHATSERVICE_H
