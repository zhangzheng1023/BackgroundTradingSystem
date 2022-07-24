#ifndef __TRADE_H_
#define __TRADE_H_
#include <drogon/HttpController.h>
#include "trade_operate.h"
using namespace drogon;

class Trade_service : public HttpController<Trade_service>
{
public:
    Trade_service();
    METHOD_LIST_BEGIN
    METHOD_ADD(Trade_service::module_operate, "", Post);
    METHOD_LIST_END
    void module_operate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    Json::Value push_store(Json::Value &json);
    Json::Value pop_store(Json::Value &json);
    Json::Value trade(Json::Value &json);
    Json::Value get_store_info(Json::Value &json);
    Json::Value get_bill_record(Json::Value &json);

private:
    std::unordered_map<std::string ,std::function<Json::Value (Json::Value &)> > func_map;
    Trade_operate trade_op;
    HttpResponsePtr make_fail_response(std::string fail_reason);
    void make_fail_json(Json::Value &json, std::string fail_reason);
    void make_ok_json(Json::Value &json, Json::Value &data);
    HttpResponsePtr make_ok_response(Json::Value &data);

};


#endif