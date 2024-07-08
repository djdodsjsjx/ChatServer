#include "usermodel.hpp"
#include "db.h"
#include <iostream>

bool UserModel::insert(User& user) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {  // 插入数据库
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true; 
        }
    }

    return false;
}

User UserModel::query(int id) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select * from user where id = '%d'", id);
    
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row) {  // 能够查到这一行
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

bool UserModel::updateState(User user) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    
    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {  // 插入数据库
            return true;
        }
    }
    return false;
}

void UserModel::resetState() {
    char sql[1024] = "update user set state = 'offline' where state = 'online'";
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}