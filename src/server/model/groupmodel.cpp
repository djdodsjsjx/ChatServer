#include "group_model.hpp"
#include "db.h"

bool GroupModel::createGroup(Group& group) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into allgroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());
    
    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            group.setId(mysql_insert_id(mysql.getConnection()));  // 跟新组id
            return true;
        }
    }
    return false;
}

void GroupModel::addGroup(int userid, int groupid, string role) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into groupuser values(%d, %d, '%s')", groupid, userid, role.c_str());

    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

vector<Group> GroupModel::queryGroups(int userId) {
    // 1. 通过gourpuser表查找userId所在的组信息
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select a.id, a.groupname, a.groupdec from allgroup a inner join groupuser b on a.id = b.groupid where b.userid=%d", userId);
    vector<Group> groupVec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res)) {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                // 2. 获取该组成员信息
                vector<GroupUser> groupUserVec = queryGroupUsers(atoi(row[0]));
                group.setUsers(groupUserVec);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}


vector<GroupUser> GroupModel::queryGroupUsers(int groupid) {
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select a.id, a.name, a.state, b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid=%d", groupid);

    vector<GroupUser> groupUserVec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res)) {
                GroupUser groupuser;
                groupuser.setId(atoi(row[0]));
                groupuser.setName(row[1]);
                groupuser.setState(row[2]);
                groupuser.setRole(row[3]);
                groupUserVec.push_back(groupuser);
            }
            mysql_free_result(res);
        }
    }
    return groupUserVec;
}

vector<int> GroupModel::queryGroupUsers(int userId, int groupid) {
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userId);
    
    vector<int> idVec;
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res) {
            MYSQL_ROW row;
            while (row = mysql_fetch_row(res)) {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}