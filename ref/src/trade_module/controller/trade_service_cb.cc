#include "trade_service.h"

Trade_service::Trade_service(){
    func_map["push_store"] = std::bind(&Trade_service::push_store,this,std::placeholders::_1,std::placeholders::_2);
    func_map["pop_store"] = std::bind(&Trade_service::pop_store,this,std::placeholders::_1,std::placeholders::_2);
    func_map["get_store_info"] = std::bind(&Trade_service::get_store_info,this,std::placeholders::_1,std::placeholders::_2);
    func_map["trade"] = std::bind(&Trade_service::trade,this,std::placeholders::_1,std::placeholders::_2);
    func_map["get_bill_record"] = std::bind(&Trade_service::get_bill_record,this,std::placeholders::_1,std::placeholders::_2);

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

    auto json_ptr = req->jsonObject();
    if (json_ptr == nullptr)
    {
        callback(make_fail_response("no json data"));
        return;
    }
    std::string func_name = (*json_ptr)["func_name"].asString();
    Json::Value &argv = (*json_ptr)["argv"];
    if(func_name==""){
        callback(make_fail_response("no function name"));
        return;
    }
    func_map[func_name](argv,callback);
}

void Trade_service::make_fail_json(Json::Value &json,std::string fail_reason){
    json["status"] = "fail";
    json["reason"] = fail_reason;
}

HttpResponsePtr Trade_service::make_fail_response(std::string fail_reason)
{
    Json::Value json;
    make_fail_json(json,std::move(fail_reason));
    auto response_ptr = HttpResponse::newHttpJsonResponse(json);
    return response_ptr;
}

void Trade_service::make_ok_json(Json::Value &json, Json::Value &data){
    json["status"] = "ok";
    json["data"] = data;
}

HttpResponsePtr Trade_service::make_ok_response(Json::Value &data)
{
    Json::Value json;
    make_ok_json(json,data);
    auto response_ptr = HttpResponse::newHttpJsonResponse(json);
    response_ptr->setStatusCode(k200OK);
    return response_ptr;
}

void Trade_service::push_store(Json::Value &json, callback_t cb){
    Json::Value user_id = json["user_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value num = json["num"];
    Json::Value res_json;
    if(user_id.isNull()||tool_id.isNull()||num.isNull()){
        LOG_DEBUG << "push store need user_id, tool_id and num";
        cb(make_fail_response("need user_id tool_id num"));
        return ;
    }

    
    LOG_DEBUG << "push user "<<user_id.asInt()<<" tool "<<tool_id.asInt()<<" num "<<num.asInt();
}
void Trade_service::pop_store(Json::Value &json,callback_t cb){
    Json::Value user_id = json["user_id"];
    Json::Value tool_id = json["tool_id"];
    Json::Value num = json["num"];
    Json::Value res_json;
    if(user_id.isNull()||tool_id.isNull()||num.isNull()){
        cb(make_fail_response("need user_id tool_id num"));
        return ;
    }

    LOG_DEBUG << "push user "<<user_id.asInt()<<" tool "<<tool_id.asInt()<<" num "<<num.asInt();

    res_json["data"] = "success"; 
    return ;
}
void Trade_service::get_store_info(Json::Value &json,callback_t cb){
    std::string user_id = json["user_id"].asString();
    std::string tool_id = json["tool_id"].asString();


    LOG_DEBUG << "get info user "<<user_id<<" tool "<<tool_id;
}
void Trade_service::trade(Json::Value &json,callback_t cb){
    std::string from_id = json["from_id"].asString();
    std::string to_id= json["to_id"].asString();
    std::string tool_id = json["tool_id"].asString();
    int num = json["num"].asInt();

    LOG_DEBUG << "trade user "<<from_id<<" to "<<to_id<<" tool "<<tool_id<<" num "<<num;
}

void Trade_service::get_bill_record(Json::Value &json,callback_t cb){
    std::string from_id = json["from_id"].asString();
    std::string to_id= json["to_id"].asString();
    std::string tool_id = json["tool_id"].asString();
    int num = json["num"].asInt();

    LOG_DEBUG << "trade user "<<from_id<<" to "<<to_id<<" tool "<<tool_id<<" num "<<num;
}