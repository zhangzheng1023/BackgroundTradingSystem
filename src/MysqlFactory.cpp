#include "mysql.h"
#include <iostream>

MysqlInstance::MysqlInstance(string &host, string &user, string &password, string &dbName) {
    mysql = mysql_init(NULL);
    _host = host;
    _user = user;
    _password = password;
    _dbName = dbName;
}

MysqlInstance::~MysqlInstance() {
    if (mysql != NULL) {
        mysql_close(mysql);
        mysql = NULL;
    }
}

bool MysqlInstance::initSql() {
    mysql = mysql_real_connect(mysql, _host, _user, _password, _dbName, 0, NULL, 0);
    if (mysql == NULL) {
        return false;
    }
    return true;
}

sqlRes MysqlInstance::execSql(string &sql) {
    sqlRes execSqlResponse = {false, 0, NULL};
    int numRowsAffect = 0;
    int retryTime = 0;
    while (mysql_query(mysql, sql) && retryTime < 4) {
        ++retryTime;
        freeSql();
        initSql();
    }
    if (retryTime >= 4) return execSqlResponse;
    result = mysql_store_result(mysql);
    if (result == NULL) {
        if (*mysql_error(mysql) == '\0') {
            numRowsAffect = mysql_affected_rows(mysql);
        }else {
            return execSqlResponse;
        }
    }
    execSqlResponse.res = true;
    execSqlResponse.affectNum = numRowsAffect;
    execSqlResponse.result = result;
    return execSqlResponse;
}

void MysqlInstance::freeSql() {
    if (mysql != NULL) {
        mysql_free_result(result);
        mysql_close(mysql);
        mysql = NULL;
    }
}
