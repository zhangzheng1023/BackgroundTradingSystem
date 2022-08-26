#include <iostream>
#include "mysql_conn.h"
using namespace std;

int main() {
    cout << "====----DataBaseTest----====" << endl;
    char *host = "localhost";
    char *user = "root";
    char *password = "092623";
    char *db_name = "test_db";
    MySql MySql_test(host, user, password, db_name);
    MySql_test.init_mysql();
    char *query_sql = "select * from user_wallet";
    MySql_test.mysql_exec(query_sql);
    MySql_test.delete_mysql();
    return 0;
}