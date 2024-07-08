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
    _msgHandlerMap.insert({REGISTER_MSG, std::bind(&ChatService::registerHandler, this, _1, _2, _3)});
}

// {"msgId":1002,"id":22,"password":"123456"}
void ChatService::loginHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    LOG_INFO << "do login service!";
    
    json response;  // 登录消息反馈
    response["msgId"] = LOGIN_MSG_ACK;
    
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == password) {
        if (user.getState() == "online") {
            response["errno"] = 2;
            response["errmsg"] = "this count is using, input anther!";
        } else {
            
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});  // 登录成功保存对应的连接
            }

            user.setState("online");
            _userModel.updateState(user);
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
        }
    } else {
        response["errno"] = 3;
        response["errmsg"] =  "password is errno!";
    }
    conn->send(response.dump());
}

// {"msgId":1000,"name":"123","password":"123456"}
void ChatService::registerHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    LOG_INFO << "do register service!";
    
    json response;  // 注册消息反馈
    response["msgId"] = REGISTER_MSG_ACK;

    string name = js["name"];
    string password = js["password"];
    User user;
    user.setName(name);
    user.setPassword(password);
    if (_userModel.insert(user)) { // 数据库插入成功
        response["errno"] = 0;
        response["id"] = user.getId();
        LOG_INFO << "insert success!";
    } else {
        response["errno"] = 1;
        LOG_INFO << "insert fail!";
    }
    // conn->send(response.dump());
}

MsgHandler ChatService::getHandler(int msgId) {
    if (!_msgHandlerMap.count(msgId)) {
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp) {
            LOG_ERROR << "msgId: " << msgId << " can not find handler!";
        };
    }
    return _msgHandlerMap[msgId];
}


void ChatService::reset() {
    _userModel.resetState();  // 将数据库中在线用户重置为离线用户
} 

void ChatService::clientCloseExceptionHandler(const TcpConnectionPtr& conn) {
    User user; 
    // 互斥锁保护
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto [id, _conn] : _userConnMap) {
            if (_conn == conn) {
                user.setId(id);
                _userConnMap.erase(id);  // 删除该用户对应的通信连接
                break;
            }
        }
    }

    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.updateState(user);  // 更新数据库
    }
    
}