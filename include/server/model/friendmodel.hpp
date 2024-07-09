#ifndef FRIENDMODEL_H
#define FRIEDNMODEL_H
#include "user.hpp"
#include <vector>
using namespace std;

class FriendModel {
public:
    // 添加好友关系
    void insert(int userId, int friendId);
    
    // 返回用户好友列表
    vector<User> query(int userId);
};

#endif // !FRIENDMODEL_H
