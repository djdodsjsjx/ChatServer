#ifndef OFFLINE_MESSAGE_MODEL_H
#define OFFLINE_MESSAGE_MODEL_H

#include <string>
#include <vector>
using namespace std;

class OfflineMsgModel
{
public:
    // 存储离线消息
    void insert(int usrId, string msg);
    
    // 删除用户的离线消息
    void remove(int userId); 
    
    // 查询用户的离线消息
    vector<string> query(int userId);
};



#endif // !OFFLINE_MESSAGE_MODEL_H