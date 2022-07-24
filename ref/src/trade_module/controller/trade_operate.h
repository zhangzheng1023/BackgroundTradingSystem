#ifndef __TRADE_OPERATE_H
#define __TRADE_OPERATE_H
#include <drogon/drogon.h>
#include "http_client.h"
#include "trade_db_client.h"

// 处理业务逻辑，非json数据
enum TRADE_STATUS {
    LOCK_FAIL=-100,
    UNLOCK_FAIL,
    SELECT_BILL_FAIL,
    SELECT_STORE_FAIL,
    ADD_STORE_FAIL,
    DEL_STORE_FAIL_NO_NUM,
    UPDATE_STORE_FAIL,
    ADD_USER_TOOL_FAIL,
    DEL_USER_TOOL_FAIL,
    CHANGE_MONEY_FAIL,
    INSERT_BILL_FAIL,
    OK=1
};
enum TRADE_PROCEDURE{
    START,
    LOCK_STORE,
    CHANGE_MONEY,
    DEL_STORE,
    ADD_USER_TOOL,
    INSERT_BILL,
};
class Trade_operate
{
public:
    Trade_operate(const char *file_path = "../config_service.json");
    ~Trade_operate();
    int push_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num, MONEY &price); //上架
    int pop_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num); //下架
    int trade( USER_ID from_id,USER_ID to_id, TOOL_ID tool_id, TOOL_NUM num); //购买
    Json::Value get_store_info(USER_ID user_id, TOOL_ID tool_id); //查看上架信息
    Json::Value get_bill_record(USER_ID from_id,USER_ID to_id, TOOL_ID tool_id); //查看交易流水

private:
    Tool_client *tool_client;
    Bank_client *bank_client;
    Db_client *db_client;
    std::string make_url(Json::Value json);
    //int remove_tool_from_store_safe(TRADE_ID trade_id, USER_ID user_id, TOOL_ID tool_id); 
};

#endif