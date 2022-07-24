#include "trade_db_client.h"
#include <trantor/utils/Logger.h>

Db_client::Db_client(Json::Value mysql_config)
{
    string host = mysql_config["host"].asString();
    string user = mysql_config["user"].asString();
    string password = mysql_config["password"].asString();
    string db_name = mysql_config["db_name"].asString();
    mysql_client = new Mysql_base(host, user, password, db_name);
}
Db_client::~Db_client()
{
    delete mysql_client;
}

int Db_client::get_bill(USER_ID from_id,USER_ID to_id, TOOL_ID tool_id, Json::Value &result)
{
    Json::Value row_json;
    sql_res sql_res_obj;
    string sql= "SELECT * from transcation_t";
    string condition="";
    string condition_start="";
    if(from_id!=0){
        condition = condition+condition_start+ " seller_id= "+to_string(from_id);
        condition_start = " and ";
    }
    if(to_id!=0){
        condition = condition+condition_start+ " buyer_id= "+to_string(to_id);
        condition_start = " and ";
    }
    if(tool_id!=0){
        condition = condition+condition_start+ " props_id= "+to_string(tool_id);
        condition_start = " and ";
    }
    if(condition != "")sql = sql+" where " + condition;
    sql_res_obj = mysql_client->exeSQL(sql);
    if (sql_res_obj.res)
    {
        int num_fields = mysql_num_fields(sql_res_obj.result); //获取结果集总共的字段数，即列数
        int num_rows = mysql_num_rows(sql_res_obj.result);     //行数
        for (int i = 0; i < num_rows; i++)                     //输出每一行
        {
            //获取下一行数据
            MYSQL_ROW row = mysql_fetch_row(sql_res_obj.result);
            if (row < 0)
                break;

            row_json["transaction_id"] = stoi(row[0]);
            row_json["buyer_id"] = stoi(row[1]);
            row_json["seller_id"] = stoi(row[2]);
            row_json["props_id"] = stoi(row[3]);
            row_json["props_number"] = stoi(row[4]);
            row_json["props_price"] = stoi(row[5]);
            row_json["total_price"] = stoi(row[6]);
            row_json["create_time"] = row[7];
            row_json["pay_time"] = stoi(row[8]);
            result.append(row_json);
        }
        return 1;
    }
    else
    {
        LOG_ERROR << "can not connect db: "+sql;
        return -1;
    }
}

int Db_client::get_store_info(USER_ID user_id, TOOL_ID tool_id, Json::Value &result)
{
    Json::Value row_json;
    sql_res sql_res_obj;
    if (user_id == 0 && tool_id == 0) //全部检索
    {
        string sql = "SELECT * from on_sale_t";
        sql_res_obj = mysql_client->exeSQL(sql);
    }
    else if (user_id != 0 && tool_id == 0)
    {
        string sql = "SELECT * from on_sale_t where user_id = ";
        sql = sql + to_string(user_id) + ";";
        sql_res_obj = mysql_client->exeSQL(sql);
    }
    else if (user_id == 0 && tool_id != 0)
    {
        string sql = "SELECT * from on_sale_t where tool_id = ";
        sql = sql + to_string(tool_id) + ";";
        sql_res_obj = mysql_client->exeSQL(sql);
    }
    else if (user_id != 0 && tool_id != 0)
    {
        string sql = "SELECT * from on_sale_t where user_id = ";
        sql = sql + to_string(user_id) + " and " + "tool_id = " + to_string(tool_id) + ";";
        sql_res_obj = mysql_client->exeSQL(sql);
    }
    if (sql_res_obj.res)
    {
        int num_fields = mysql_num_fields(sql_res_obj.result); //获取结果集总共的字段数，即列数
        int num_rows = mysql_num_rows(sql_res_obj.result);     //行数
        for (int i = 0; i < num_rows; i++)                     //输出每一行
        {
            //获取下一行数据
            MYSQL_ROW row = mysql_fetch_row(sql_res_obj.result);
            if (row < 0)
                break;
            assert(num_fields>6);
            row_json["sale_id"] = stoi(row[0]);
            row_json["user_id"] = stoi(row[1]);
            row_json["tool_id"] = stoi(row[2]);
            row_json["remain_num"] = stoi(row[3]);
            row_json["sale_time"] = row[4];
            row_json["trade_lock"] = stoi(row[5]);
            row_json["price"] = stoi(row[6]);
            result.append(row_json);
        }
        return 1;
    }
    else
    {
        LOG_ERROR << "connect db fail";
        return -1;
    }
}

std::string Db_client::get_now_time()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string time_sql = to_string(int(1900 + ltm->tm_year)) + "-" + to_string(int(1 + ltm->tm_mon)) + "-" + to_string(ltm->tm_mday) + " " + to_string(ltm->tm_hour) + ":" +
                      to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
    return std::move(time_sql);
}
int Db_client::insert_bill(USER_ID user_id, USER_ID to_id, TOOL_ID tool_id, TOOL_NUM num, MONEY money, T_MONEY t_money)
{
    sql_res sql_res_obj;
    string time_sql = get_now_time();
    string sql = "INSERT INTO transcation_t (buyer_id, seller_id, props_id, props_number, props_price, total_price, create_time, pay_time) VALUES (";
    sql = sql + to_string(to_id) + "," + to_string(user_id) + "," + to_string(tool_id) + "," + to_string(num) + "," +
          to_string(money) + "," + to_string(t_money) + "," + "'" + time_sql + "'" + "," + "'" + time_sql + "'" + ")" + ";";
    LOG_INFO << sql;
    sql_res_obj = mysql_client->exeSQL(sql);
    if (sql_res_obj.res)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int Db_client::insert_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num, MONEY price)
{
    sql_res sql_res_obj;
    string sql = "INSERT INTO on_sale_t (user_id, tool_id, remain_num,sale_time,trade_lock,price) VALUES (";
    sql = sql + to_string(user_id) + "," + to_string(tool_id) + "," + to_string(num) + ",'" + get_now_time() + "'," + to_string(0) + ","+to_string(price)+")" + ";";
    return deal_trade_sql(sql);
}

int Db_client::update_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num)
{
    sql_res sql_res_obj;
    string sql = "UPDATE on_sale_t SET remain_num = ";
    sql = sql + to_string(num) + " where user_id = " + to_string(user_id) + " and " + "tool_id = " + to_string(tool_id) + ";";
    return deal_trade_sql(sql);
}

int Db_client::del_tool_from_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num)
{
    Json::Value res_json;
    int res= get_store_info(user_id,tool_id,res_json);
    if(res<0){
        return -1;
    }
    if(res_json.size()<1){
        LOG_INFO << "user_id has no tool_id:" <<user_id <<","<<tool_id;
        return -1;
    }
    int remain_num = res_json[0]["remain_num"].asInt();
    return update_store(user_id,tool_id,remain_num-num);
}
int Db_client::add_tool_to_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num)
{
    Json::Value res_json;
    int res= get_store_info(user_id,tool_id,res_json);
    if(res<0){
        return -1;
    }
    if(res_json.size()<1){
        LOG_INFO << "user_id has no tool_id:" <<user_id <<","<<tool_id;
        return -1;
    }
    int remain_num = res_json[0]["remain_num"].asInt();
    return update_store(user_id,tool_id,remain_num-num);
}

int Db_client::lock_trade_more_num(TRADE_ID trade_id, TOOL_NUM num)
{
    string sql = "UPDATE on_sale_t SET trade_lock = 1 where trade_id = ";
    sql = sql + to_string(trade_id) + " and " + "remain_num >= " + to_string(num);
    sql = sql + "and" + "remain_num>="+to_string(num) + ";";
    return deal_trade_sql(sql);
}

int Db_client::lock_trade_more_num(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num)
{
    string sql = "UPDATE on_sale_t SET trade_lock = 1 where user_id = ";
    sql = sql + to_string(user_id) + " and " + "tool_id = " + to_string(tool_id);
    sql = sql + " and " + " remain_num>="+to_string(num) + ";";
    return deal_trade_sql(sql);
}


int Db_client::unlock_trade(TRADE_ID trade_id)
{
    sql_res sql_res_obj;
    string sql = "UPDATE on_sale_t SET trade_lock = 0 where trade_lock=1 and sale_id = ";
    sql = sql + to_string(trade_id) + ";";
    sql_res_obj = mysql_client->exeSQL(sql);
    if (sql_res_obj.res && sql_res_obj.affect_num > 0)
    {
        return 1;
    }
    else
    {

        return -1;
    }
}

int Db_client::unlock_trade(USER_ID user_id, TOOL_ID tool_id)
{
    sql_res sql_res_obj;
    string sql = "UPDATE on_sale_t SET trade_lock = 0 where user_id = ";
    sql = sql + to_string(user_id) + " and " + "tool_id = " + to_string(tool_id) + ";";
    sql_res_obj = mysql_client->exeSQL(sql);
    if (sql_res_obj.res)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int Db_client::del_store(USER_ID user_id, TOOL_ID tool_id)
{
    sql_res sql_res_obj;
    string sql = "delete from on_sale_t where user_id = ";
    sql = sql + to_string(user_id) + " and " + "tool_id = " + to_string(tool_id) + ";";
    return deal_trade_sql(sql);
}
int Db_client::del_store(TRADE_ID trade_id)
{
    sql_res sql_res_obj;
    string sql = "delete from on_sale_t where sale_id = " + to_string(trade_id) + ";";
    return deal_trade_sql(sql);
}

int Db_client::deal_trade_sql(string &sql){
    sql_res sql_res_obj;
    sql_res_obj = mysql_client->exeSQL(sql);
    if (sql_res_obj.res)
    {
        if( sql_res_obj.affect_num > 0){
            return 1;
        }
        else{
            LOG_INFO<< "exec sql no affect:" <<sql;
            return -1;
        }
    }
    else
    {
        LOG_ERROR << "can not connect to db";
        return -1;
    }
}

MONEY Db_client::get_tool_price(TOOL_ID tool_id){
    string sql= "select money from prop where id="+to_string(tool_id)+";";
    sql_res sql_res_obj;
    sql_res_obj = mysql_client->exeSQL(sql);
    if(sql_res_obj.res){
        MYSQL_ROW row = mysql_fetch_row(sql_res_obj.result);
        return stoi(row[0]);
    }
    else{
        LOG_ERROR << "connect db fail ";
        return -1;
    }
}