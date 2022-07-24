#include "trade_operate.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
Trade_operate::Trade_operate(const char *file_path)
{
    Json::Reader reader;
    Json::Value root;
    std::ifstream in;
    in.open(file_path, ios::binary);
    reader.parse(in, root);
    root = root[std::getenv("env")];
    Json::Value &tool_config = root["tool_client"];
    Json::Value &bank_config = root["bank_client"];
    Json::Value &db_config = root["db_client"];
    tool_client = new Tool_client(make_url(tool_config));
    bank_client = new Bank_client(make_url(bank_config));
    db_client = new Db_client(db_config);
}
Trade_operate::~Trade_operate()
{
    delete tool_client;
    delete bank_client;
    delete db_client;
}

std::string Trade_operate::make_url(Json::Value json)
{
    return "http://" + json["ip"].asString() + ":" + json["port"].asString() + json["service"].asString();
}

int Trade_operate::push_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num, MONEY &price)
{
    int res = tool_client->remove_tool(user_id, tool_id, num);
    if (res < 0)
    {
        LOG_ERROR << "remove tool for user fail";
        return DEL_USER_TOOL_FAIL;
    }
    Json::Value res_json;
    res = db_client->get_store_info(user_id, tool_id, res_json);
    if (res < 0)
    {
        LOG_ERROR << "add tool to store fail";
        return SELECT_STORE_FAIL;
    }
    Json::FastWriter fastwrite;
    LOG_INFO << fastwrite.write(res_json);
    if (res_json.size() > 0)
    {

        TOOL_NUM remain_num = res_json[0]["remain_num"].asInt();
        res = db_client->update_store(user_id, tool_id, num + remain_num);
    }
    else
    {
        if (price == 0)
        {
            price = db_client->get_tool_price(tool_id);
            if (price < 0)
            {
                LOG_ERROR << "push fail ,cannot get price";
                goto back;
            }
        }
        res = db_client->insert_store(user_id, tool_id, num, price);
    }
    if (res < 0)
    {
    back:
        LOG_ERROR << "add tool to store fail";
        int res = tool_client->add_tool(user_id, tool_id, num);
        return UPDATE_STORE_FAIL;
    }
    return OK;
}

/*
先下架再给用户,下架的时候注意是否锁上
*/
int Trade_operate::pop_store(USER_ID user_id, TOOL_ID tool_id, TOOL_NUM num)
{
    int res = db_client->lock_trade_more_num(user_id, tool_id, num);
    if (res < 0)
    {
        LOG_INFO << "store is lock this tool";
        return TRADE_STATUS::LOCK_FAIL;
    }
    LOG_INFO << res;
    Json::Value res_json;
    res = db_client->get_store_info(user_id, tool_id, res_json);
    if (res < 0)
    {
        LOG_ERROR << "get store info fail";
        res = db_client->unlock_trade(user_id, tool_id);
        if (res < 0)
        {
            LOG_ERROR << "unlock tool fail ";
        }
        return SELECT_STORE_FAIL;
    }
    if (res_json.size() < 0)
    {
        LOG_ERROR << "remove tool from store fail, no tool";
        res = db_client->unlock_trade(user_id, tool_id);
        if (res < 0)
        {
            LOG_ERROR << "unlock tool fail ";
        }
        return DEL_STORE_FAIL_NO_NUM;
    }
    Json::FastWriter fastwrite;
    std::cout << fastwrite.write(res_json);
    int remain_num = res_json[0]["remain_num"].asInt();
    if (remain_num == num)
    {
        res = db_client->del_store(user_id, tool_id);
    }
    else
    {
        res = db_client->update_store(user_id, tool_id, remain_num - num);
    }

    if (res < 0)
    {
        LOG_INFO << "del tool from store fail";
        res = db_client->unlock_trade(user_id, tool_id);
        if (res < 0)
        {
            LOG_ERROR << "unlock tool fail ";
        }
        return UPDATE_STORE_FAIL;
    }
    res = db_client->unlock_trade(user_id, tool_id);
    if (res < 0)
    {
        LOG_ERROR << "unlock tool fail ";
    }
    res = tool_client->add_tool(user_id, tool_id, num);
    if (res < 0)
    {
        LOG_ERROR << "add tool for user fail";
        if (remain_num == num)
        {
            res = db_client->insert_store(user_id, tool_id,num,res_json["price"].asInt());
        }
        else
        {
            res = db_client->update_store(user_id, tool_id, remain_num);
        }
        return ADD_STORE_FAIL;
    }
    return 1;
}
int Trade_operate::trade(USER_ID from_id, USER_ID to_id, TOOL_ID tool_id, TOOL_NUM num)
{ //下架对商品的影响
    //forward: lock->change->del->add->insert
    //back: 0,1,2,3,4,5
    int back_num = 0, RES = OK;
    MONEY res = db_client->lock_trade_more_num(from_id, tool_id, num);
    if (res < 0)
    {
        LOG_INFO << "tool can not be buy";
        RES = LOCK_FAIL;
        return RES;
    }
    back_num++;
    Json::Value res_json;
    res = db_client->get_store_info(from_id, tool_id, res_json);
    MONEY price = 0, money = 0;
    TOOL_NUM remain_num = 0;
    if (res < 0)
    {
        goto back;
    }
    price = res_json[0]["price"].asInt();
    remain_num = res_json[0]["remain_num"].asInt();
    money = price * num;
    res = bank_client->change_money(to_id, from_id, money);
    if (res < 0)
    {
        LOG_ERROR << "user money change fail";
        RES = CHANGE_MONEY_FAIL;
        goto back;
    }
    back_num++;
    res = db_client->update_store(from_id, tool_id, remain_num - num);
    if (res < 0)
    {
        LOG_ERROR << "delete tool fail: " << res;
        RES = DEL_STORE_FAIL_NO_NUM;
        goto back;
    }
    back_num++;

    res = tool_client->add_tool(to_id, tool_id, num);
    if (res < 0)
    {
        LOG_ERROR << "add user_tool fail";
        RES = ADD_USER_TOOL_FAIL;
        goto back;
    }
    else
    {
        db_client->unlock_trade(from_id, tool_id);
    }
    back_num++;

    res = db_client->insert_bill(from_id, to_id, tool_id, num, price, money);
    if (res < 0)
    {
        LOG_ERROR << "insert bill fail";
        RES = INSERT_BILL_FAIL;
        goto back;
    }
    return OK;

back:
    while (back_num > 0)
    {
        switch (back_num)
        {
        case START:
            return RES;
        case LOCK_STORE:
            db_client->unlock_trade(from_id, tool_id);
            back_num--;
            break;
        case CHANGE_MONEY:
            bank_client->change_money(from_id, to_id, money);
            back_num--;
            break;
        case DEL_STORE:
            db_client->update_store(from_id, tool_id, remain_num);
            back_num--;
            break;
        case ADD_USER_TOOL:
            tool_client->remove_tool(to_id, tool_id, num);
            back_num--;
            break;
        default:
            break;
        }
    }
    return RES;
}
Json::Value Trade_operate::get_store_info(USER_ID user_id, TOOL_ID tool_id)
{
    Json::Value res_json;
    int res = db_client->get_store_info(user_id, tool_id, res_json);
    if (res > 0)
    {
        return res_json;
    }
    else
    {
        LOG_ERROR << "get store info fail ";
        return Json::Value();
    }
}
Json::Value Trade_operate::get_bill_record(USER_ID from_id, USER_ID to_id,TOOL_ID tool_id)
{
    Json::Value res_json;
    int res = db_client->get_bill(from_id,to_id, tool_id, res_json);
    if (res > 0)
    {
        return res_json;
    }
    else
    {
        LOG_ERROR << "get bill record fail ";
        return Json::Value();
    }
}