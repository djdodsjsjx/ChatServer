#include "friendmodel.hpp"
#include "db.h"

void FriendModel::insert(int userId, int friendId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into friend values(%d, %d)", userId, friendId);
    
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

// 查询userId好友，并返回好友列表
vector<User> FriendModel::query(int userId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userId);
    
    vector<User> vec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res)) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}