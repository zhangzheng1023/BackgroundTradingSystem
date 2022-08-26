#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <mysql/mysql.h>

class MySql {
public:
    MySql(char *host, char *user, char *password, char *db_name);  // 构造函数 传入创建连接的必要参数
    bool init_mysql();  // 使用传入参数对其进行初始化
    void delete_mysql();  // 释放资源
    void mysql_exec(char *sql);

private:
    MYSQL *mysql; // 建立连接的MYSQL指针
    MYSQL_RES *result; //指向查询结果的指针
    char *_host, *_user, *_password, *_db_name;
};

#endif