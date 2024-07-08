#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User
{
private:
    int _id;
    string _name;
    string _password;
    string _state;
    
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    : _id(id), _name(name), _password(pwd), _state(state) {}
    void setId(const int& id) {_id = id;}
    void setName(const string& name) {_name = name;}
    void setPassword(const string& pwd) {_password = pwd;}
    void setState(const string& state) {_state = state;}
    
    int getId() {return _id;}
    string getName() {return _name;}
    string getPassword() {return _password;}
    string getState() {return _state;}
};


#endif // !USER_H
