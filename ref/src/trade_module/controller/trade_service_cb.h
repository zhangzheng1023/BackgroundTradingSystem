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
    METHOD_ADD(Trade_service::module_operate, "/module_operate", Post);
    METHOD_LIST_END
    typedef std::function<void(const HttpResponsePtr &)> callback_t;
    void module_operate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void push_store(Json::Value &json,callback_t cb);
    void pop_store(Json::Value &json,callback_t cb);
    void trade(Json::Value &json,callback_t cb);
    void get_store_info(Json::Value &json,callback_t cb);
    void get_bill_record(Json::Value &json,callback_t cb);

private:
    std::unordered_map<std::string ,std::function<void(Json::Value &, callback_t )> > func_map;
    Trade_operate trade_op;
    HttpResponsePtr make_fail_response(std::string fail_reason);
    void make_fail_json(Json::Value &json, std::string fail_reason);
    void make_ok_json(Json::Value &json, Json::Value &data);
    HttpResponsePtr make_ok_response(Json::Value &data);

};

#define parse(json,value1,value2, value3) do{\
    value1 = json[#value1];\
    value2 = json[#value2];\
    value3 = json[#value3];\
    if(value1.isNull()||value2.isNull()||value3.isNull())return res_json;}while(0);

#endif