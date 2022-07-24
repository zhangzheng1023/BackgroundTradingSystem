#pragma once

#ifndef _MYSQLMANAGER_H
#define _MYSQLMANAGER_H

#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include "json/json.h"

using namespace std;

struct sql_res
{
    bool res;
    int affect_num;
    MYSQL_RES *result;
};

class Mysql_base
{
public:
    Mysql_base(string& host, string& user, string &password, string &db_name);
    ~Mysql_base();
    bool init_sql();
    struct sql_res exeSQL(string &sql); //执行sql语句
    void free_sql();

private:
    MYSQL *mysql;      //连接mysql的指针
    MYSQL_RES *result; //指向查询结果的指针
    string _host,_user,_password,_db_name;
};

#endif