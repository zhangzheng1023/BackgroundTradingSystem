#include <iostream>
#include <string>
#include "mysql_base.h"
#include <trantor/utils/Logger.h>
using namespace std;

Mysql_base::Mysql_base(string &host, string &user, string &password, string &db_name)
{
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {

        LOG_ERROR << "Error:" << mysql_error(mysql);
    }
    _host = host;
    _user = user;
    _password = password;
    _db_name = db_name;
    if (init_sql() == false)
    {
        LOG_ERROR << "can not connect db:"
                  << " host:" << host
                  << " user:" << user
                  << " password:" << password
                  << " db_name:" << db_name;
    }
    else
    {
        LOG_INFO << "connect db:"
                 << " host:" << host
                 << " user:" << user
                 << " password:" << password
                 << " db_name:" << db_name;
    }
}

Mysql_base::~Mysql_base()
{
    if (mysql != NULL)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}

bool Mysql_base::init_sql()
{
    mysql = mysql_real_connect(mysql, _host.c_str(), _user.c_str(), _password.c_str(), _db_name.c_str(), 0, NULL, 0);
    if (mysql == NULL)
    {
        LOG_ERROR << "Error:" << mysql_error(mysql);
        return false;
    }
    return true;
}

struct sql_res Mysql_base::exeSQL(string& sql)
{
    sql_res sql_res_exe = {false, 0, NULL};
    LOG_DEBUG << sql;
    int num_rows_affect = 0;
    int retry_time = 0;
    while (mysql_query(mysql, sql.c_str()) && retry_time < 4)
    {
        retry_time++;
        free_sql();
        init_sql();
    }
    if(retry_time>=4){
        LOG_ERROR << mysql_error(mysql);
        return sql_res_exe;
    }
    result = mysql_store_result(mysql);
    if (result == NULL)
    {
        if (*mysql_error(mysql) == '\0')
        {
            num_rows_affect = mysql_affected_rows(mysql);
        }
        else //error
        {
            LOG_ERROR << "Get result error:" << mysql_error(mysql);
            return sql_res_exe;
        }
    }

    sql_res_exe.res = true;
    sql_res_exe.affect_num = num_rows_affect;
    sql_res_exe.result = result;
    return sql_res_exe;
}

void Mysql_base::free_sql()
{
    if (mysql != NULL)
    {
        mysql_free_result(result);
        mysql_close(mysql);
        mysql = NULL;
    }
}
