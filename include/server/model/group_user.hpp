#ifndef __GROUP_USER_H__
#define __GROUP_USER_H__

#include "user.hpp"
#include <string>

using namespace std;
class GroupUser : public User
{
private:
    string _role;
public:
    GroupUser() = default;
    void setRole(const string& role) { _role = role; }
    string getRole() { return _role; }
};

#endif // !__GROUP_USER_H__