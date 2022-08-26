#include "mysql_conn.h"
#include <iostream>

MySql::MySql(char *host, char *user, char *password, char *db_name) {
    // 初始化MYSQL指针
    mysql = mysql_init(NULL);
    _host = host;
    _user = user;
    _password = password;
    _db_name = db_name;
}

bool MySql::init_mysql() {
    mysql = mysql_real_connect(mysql, _host, _user, _password, _db_name, 3306, NULL, 0);
    if (mysql == NULL) {
        std::cout << "MySql Initial Falied" << mysql_error(mysql) << std::endl;
        return false;
    } else {
        std::cout << "MySql Initial Succeed" << std::endl;
        return true;
    }
}

void MySql::delete_mysql() {
    if (mysql != NULL) {
        mysql_free_result(result);
        mysql_close(mysql);
        mysql = NULL;
    }
}

void MySql::mysql_exec(char *sql) {
    mysql_query(mysql, sql);
    result = mysql_store_result(mysql);
    std::cout << mysql_affected_rows(mysql) << std::endl;
}
