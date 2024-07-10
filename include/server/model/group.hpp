#ifndef __GROUP_H__
#define __GROUP_H__

#include <string>
#include <vector>
#include "group_user.hpp"

using namespace std;
class Group
{
private:
    int _id;            // 组id
    string _name;       // 组名
    string _desc;       // 组描述
    vector<GroupUser> _users;  // 组员列表
public:
    Group(int id = -1, string name = "", string desc = "")
    : _id(id), _name(name), _desc(desc) {}
    
    void setId(int id) { _id = id; }
    void setName(string name)  { _name = name; }
    void setDesc(string desc)  { _desc = desc; }
    void setUsers(vector<GroupUser>& users) { _users = users; }  // 添加成员列表

    int getId() { return _id; }
    string getName() { return _name; }
    string getDesc() { return _desc; }
    vector<GroupUser> getUsers() { return _users; }
    
};


#endif // !__GROUP_H__