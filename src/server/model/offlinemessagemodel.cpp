#include "offlinemessagemodel.hpp"
#include "db.h"

void OfflineMsgModel::insert(int userId, string msg) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into offlinemessage values(%d, '%s')", userId, msg.c_str());
    
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

void OfflineMsgModel::remove(int userId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "delete from offlinemessage where userid=%d", userId);

    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::query(int userId) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select message from offlinemessage where userid=%d", userId);
    
    vector<string> vec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res)) {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}