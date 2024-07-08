#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

class UserModel
{

public:
    bool insert(User& user);  // 用户插入
    User query(int id);  // 根据用户号码查询用户信息
    bool updateState(User user);  // 更新用户状态信息
    void resetState();  // 重置用户状态
};



#endif // !USERMODEL_H