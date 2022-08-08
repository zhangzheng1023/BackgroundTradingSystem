#ifndef MYSQLFACTORY_H
#define MYSQLFACTORY_H

#include<mysql/mysql.h>

struct sqlRes {
    bool res;
    int affectNum;
    MYSQL_RES *result;
};

class MysqlInstance {
public:
    MysqlInstance(string &host, string &user, string &password, string &dbName);
    ~MysqlInstance();
    bool initSql();
    void freeSql();
    sqlRes execSql(string &sql);
private:
    MYSQL *mysql;
    MYSQL_RES *result;
    string _host, _user, _password, _dbName;
};

#endif