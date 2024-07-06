#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// server和client的公共头文件
enum EnMsgType {
    LOGIN_MSG = 1000,  // 登录消息
    RETISTER_MSG,      // 注册消息
};

#endif // !__PROTOCOL_H__