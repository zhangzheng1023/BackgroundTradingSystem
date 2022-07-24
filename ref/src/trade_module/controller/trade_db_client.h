#ifndef __DB_CLIENT_H
#define __DB_CLIENT_H

#include "mysql_base.h"


typedef int USER_ID; 
typedef int TRADE_ID;
typedef int TOOL_ID;
typedef double MONEY;
typedef int BILL_ID;
typedef int TOOL_NUM;
typedef double T_MONEY;


class Db_client
{
public:
    Db_client(Json::Value mysql_config);
    ~Db_client();
    int get_bill(USER_ID from_id,USER_ID to_id,TOOL_ID tool_id, Json::Value &result);
    int get_store_info(USER_ID user_id, TOOL_ID tool_id, Json::Value &result);
    int insert_bill(USER_ID user_id, USER_ID to_id, TOOL_ID tool_id, TOOL_NUM num, MONEY money, T_MONEY t_money); //获取账单
    int update_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num);//如果有，那么是 添加 的过程
    int add_tool_to_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num);//加锁不能删除，说明在交易
    int del_tool_from_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num);//加锁不能删除，说明在交易
    int lock_trade_more_num(TRADE_ID trade_id, TOOL_NUM num);//如果trade_id的值大于num，那么锁起来，返回1，否则返回0，必须实现为原子操作
    int lock_trade_more_num(USER_ID user_id,TOOL_ID tool_id, TOOL_NUM num);//如果trade_id的值大于num，那么锁起来，返回1，否则返回0，必须实现为原子操作
    int unlock_trade(TRADE_ID trade_id);
    int unlock_trade(USER_ID user_id, TOOL_ID tool_id);
    int insert_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num,MONEY price);
    int del_store(USER_ID user_id, TOOL_ID tool_id);
    int del_store(TRADE_ID trade_id);
    MONEY get_tool_price(TOOL_ID tool_id);
private:
    std::string get_now_time();
    int deal_trade_sql(string &sql);
    Mysql_base *mysql_client;
};


#endif