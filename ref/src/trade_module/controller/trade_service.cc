#include "trade_service.h"
#include "http_client_cb.h"
#include "unistd.h"
Trade_service::Trade_service()
{
    func_map["push_store"] = std::bind(&Trade_service::push_store, this, std::placeholders::_1);
    func_map["pop_store"] = std::bind(&Trade_service::pop_store, this, std::placeholders::_1);
    func_map["get_store_info"] = std::bind(&Trade_service::get_store_info, this, std::placeholders::_1);
    func_map["trade"] = std::bind(&Trade_service::trade, this, std::placeholders::_1);
    func_map["get_bill_record"] = std::bind(&Trade_service::get_bill_record, this, std::placeholders::_1);
}

void Trade_service::module_operate(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    /*
    {
        "func_name":"push_store",
        "argv":{
            "user_id":""
        }
    }
    */
    LOG_INFO << req->bodyData();
    shared_ptr<Json::Value> json_ptr;
    if (req->contentType() == drogon::ContentType::CT_APPLICATION_JSON)
    {
        json_ptr = req->jsonObject();
        if (json_ptr == nullptr)
        {
            LOG_WARN << "no json data";
            callback(make_fail_response("no json data"));
            return;
        }
    }
    else
    {
        make_fail_response("no json data, or header not application/json");
    }

    std::string func_name = (*json_ptr)["func_name"].asString();
    Json::Value &argv = (*json_ptr)["argv"];

    auto func_it = func_map.find(func_name);
    if (func_it == func_map.end())
    {
        LOG_ERROR << "no function name :<" << func_name << ">";
        callback(make_fail_response("no function name"));
        return;
    }
    Bank_client_cb bank_client("http://127.0.0.1:8086/update/money");
    auto func = func_it->second;
    std::string body_data = req->bodyData();
    bank_client.change_money(0, 0, 0, [=](Json::Value json, int res)
                             {
                                 Json::Reader reader;
                                 Json::Value argv;
                                 if(reader.parse(body_data.c_str(), argv)==false){
                                     LOG_INFO << "parse data fail";
                                     return ;
                                 }
                                 Json::Value res_json = func(argv["argv"]);
                                 callback(HttpResponse::newHttpJsonResponse(res_json));
                             });
}

void Trade_service::make_fail_json(Json::Value &json, std::string fail_reason)
{
    json["status"] = "fail";
    json["reason"] = fail_reason;
}

HttpResponsePtr Trade_service::make_fail_response(std::string fail_reason)
{
    Json::Value json;
    make_fail_json(json, std::move(fail_reason));
    auto response_ptr = HttpResponse::newHttpJsonResponse(json);
    return response_ptr;
}

void Trade_service::make_ok_json(Json::Value &json, Json::Value &data)
{
    json["status"] = "ok";
    json["data"] = data;
}

HttpResponsePtr Trade_service::make_ok_response(Json::Value &data)
{
    Json::Value json;
    make_ok_json(json, data);
    auto response_ptr = HttpResponse::newHttpJsonResponse(json);
    response_ptr->setStatusCode(k200OK);
    return response_ptr;
}

/*
需要多次回调
*/
Json::Value Trade_service::push_store(Json::Value &json)
{
    Json::Value user_id = json["user_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value num = json["num"];
    MONEY price = json["price"].asDouble();
    Json::Value res_json;
    if (user_id.isNull() || tool_id.isNull() || num.isNull())
    {
        LOG_ERROR << "push store fail.need argvs "
                  << " user_id:" << user_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt();
        make_fail_json(res_json, "need user_id tool_id num");
        return res_json;
    }
    int res = trade_op.push_store(user_id.asInt(), tool_id.asInt(), num.asInt(), price);
    if (res > 0)
    {
        LOG_INFO << "push store ok. "
                 << " user_id:" << user_id.asInt()
                 << " tool_id:" << tool_id.asInt()
                 << " num:" << num.asInt()
                 << " price: " << price;
        make_ok_json(res_json, json);
    }
    else
    {
        LOG_ERROR << "push store fail. "
                  << " user_id:" << user_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt()
                  << " price: " << price;
        make_fail_json(res_json, "fail push");
    }
}
Json::Value Trade_service::pop_store(Json::Value &json)
{
    Json::Value user_id = json["user_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value num = json["num"];
    Json::Value res_json;
    if (user_id.isNull() || tool_id.isNull() || num.isNull())
    {
        LOG_ERROR << "pop store fail. need argvs "
                  << " user_id:" << user_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt();
        make_fail_json(res_json, "need user_id tool_id num");
        return res_json;
    }

    LOG_DEBUG << "pop user " << user_id.asInt() << " tool " << tool_id.asInt() << " num " << num.asInt();
    int res = trade_op.pop_store(user_id.asInt(), tool_id.asInt(), num.asInt());
    if (res > 0)
    {
        LOG_INFO << "pop store ok. "
                 << " user_id:" << user_id.asInt()
                 << " tool_id:" << tool_id.asInt()
                 << " num:" << num.asInt();
        make_ok_json(res_json, json);
    }
    else
    {
        LOG_ERROR << "pop store fail. "
                  << " user_id:" << user_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt();
        make_fail_json(res_json, "fail push");
    }
    return res_json;
}

Json::Value Trade_service::get_store_info(Json::Value &json)
{
    Json::Value user_id = json["user_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value res_json;

    // TODO : 需要适配
    Json::Value data = trade_op.get_store_info(user_id.asInt(), tool_id.asInt());
    make_ok_json(res_json, data);
    return res_json;
}

Json::Value Trade_service::trade(Json::Value &json)
{
    Json::Value trade_id = json["trade_id"];
    Json::Value from_id = json["from_id"];
    Json::Value to_id = json["to_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value num = json["num"];
    Json::Value res_json;
    if (from_id.isNull() || to_id.isNull() || tool_id.isNull() || num.isNull())
    {
        LOG_ERROR << "trade info is not complete "
                  << " from_id:" << from_id.asInt()
                  << " to_id:" << to_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt();
        make_fail_json(res_json, "trade info is not complete");
        return res_json;
    }

    // TODO
    int res = trade_op.trade(from_id.asInt(), to_id.asInt(), tool_id.asInt(), num.asInt());

    if (res < 0)
    {
        LOG_ERROR << "trade fail "
                  << " from_id:" << from_id.asInt()
                  << " to_id:" << to_id.asInt()
                  << " tool_id:" << tool_id.asInt()
                  << " num:" << num.asInt();
        make_fail_json(res_json, "trade fail");
        return res_json;
    }
    Json::Value data = json;
    make_ok_json(res_json, data);
    return res_json;
}

Json::Value Trade_service::get_bill_record(Json::Value &json)
{
    Json::Value from_id = json["from_id"];
    Json::Value to_id = json["to_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value res_json;
    if (tool_id.isNull() && from_id.isNull() && tool_id.isNull())
    {
        LOG_WARN << "need from_id,or tool_id or to_id,or all them";
        make_fail_json(res_json, "need from_id,or tool_id or to_id,or all them");
        return res_json;
    }
    //TODO
    Json::Value data = trade_op.get_bill_record(from_id.asInt(), to_id.asInt(), tool_id.asInt());
    // 无论如何调用都是成功的
    make_ok_json(res_json, data);
    return res_json;
}
