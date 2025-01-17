#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// server和client的公共头文件
enum EnMsgType {
    REGISTER_MSG = 1000,   // 注册消息
    REGISTER_MSG_ACK,      // 注册相应消息
    LOGIN_MSG,             // 登录消息
    LOGIN_MSG_ACK,         // 登录相应消息
    ONE_CHAT_MSG,          // 聊天消息
    ADD_FRIEND_MSG,        // 添加好友消息
    CREATE_GROUP_MSG,      // 创建群组
    ADD_GROUP_MSG,         // 加入群组
    GROUP_CHAT_MSG,        // 群聊天
    LOGOUT_MSG,         // 用户退出消息
};

#endif // !__PROTOCOL_H__