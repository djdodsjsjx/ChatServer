#ifndef __GROUP_MODEL_H__
#define __GROUP_MODEL_H__

#include "group.hpp"
#include <string>
#include <vector>

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group& group);
    
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    
    // 查询用户所在群组信息
    vector<Group> queryGroups(int userId);

    // 查村指定用户指定群聊下的用户列表(不包括自己)
    vector<int> queryGroupUsers(int userId, int groupid);

private:
    vector<GroupUser> queryGroupUsers(int groupid);
};

#endif // !__GROUP_MODEL_H__