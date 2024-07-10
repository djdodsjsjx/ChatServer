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
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChatHandler, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriendHandler, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGOUT_MSG, std::bind(&ChatService::logout, this, _1, _2, _3)});
}

// {"msgId":1002,"id":23,"password":"123456"}
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

            // 查询该用户是否存在离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            } else {
                LOG_INFO << "none offlinemsg";
            }

            vector<User> userVec = _friendModel.query(id);
            LOG_INFO << userVec.size();
            if (!userVec.empty()) {
                vector<string> tmp;
                for (auto& user : userVec) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    tmp.push_back(js.dump());
                }
                response["friends"] = tmp;
            }
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
    conn->send(response.dump());
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

// {"msgId":1004,"fromid":22,"toid":23,"msg":"hello"}
void ChatService::oneChatHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int toId = js["toid"].get<int>();  // 获取转发Id
    
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if (it != _userConnMap.end()) {  // 若该用户在线直接转发
            it->second->send(js.dump());
            return ;
        }
    }

    _offlineMsgModel.insert(toId, js.dump());
}

// {"msgId":1005,"id":22,"friendId":23,"msg":"add friend"}
void ChatService::addFriendHandler(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userId = js["id"].get<int>();
    int friendId = js["friendId"].get<int>();
    
    // 存储好友信息
    _friendModel.insert(userId, friendId);
}

// {"msgId":1006,"id":22,"groupname":"zhys","groupdesc":"zhy group"}
void ChatService::createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userId = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) {  // 创建成功，设置创建人
        _groupModel.addGroup(userId, group.getId(), "creator");
    }
}

// {"msgId":1007,"id":23,"groupid":3}
void ChatService::addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    _groupModel.addGroup(userId, groupId, "normal");
}

// {"msgId":1008,"id":23,"groupid":3, "msg":"hello group"}
void ChatService::groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userId = js["id"].get<int>();
    int groupId = js["groupid"].get<int>();
    vector<int> userIdVec = _groupModel.queryGroupUsers(userId, groupId);  // 获取其他用户Id

    lock_guard<mutex> lock(_connMutex);
    for (int id : userIdVec) {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()) {  // 转发群消息
            it->second->send(js.dump());
        } else {
            _offlineMsgModel.insert(id, js.dump());  // 不在线保存至离线消息中
        }
    }
}

void ChatService::logout(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userId = js["id"].get<int>();
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        user.setId(userId);
        _userConnMap.erase(userId);  // 删除该用户对应的通信连接
    }

    user.setState("offline");
    _userModel.updateState(user);  // 更新数据库
}