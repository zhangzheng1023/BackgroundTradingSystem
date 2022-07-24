#include "http_client_cb.h"
#include <unistd.h>
#include <curl/curl.h>
Http_client_cb::Http_client_cb(std::string host_string)
{
    url = host_string;
    client_ptr = drogon::HttpClient::newHttpClient(host_string);
    req_ptr = drogon::HttpRequest::newHttpRequest();
    req_ptr->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req_ptr->setMethod(drogon::Post);
}

Http_client_cb::Http_client_cb(std::string &ip, uint16_t port)
{
    url = "http://"+ip+":"+std::to_string(port);
    client_ptr = drogon::HttpClient::newHttpClient(ip, port);
    req_ptr = drogon::HttpRequest::newHttpRequest();
    req_ptr->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req_ptr->setMethod(drogon::Post);
}

void Http_client_cb::request(Json::Value json,std::function<void (Json::Value data,int res)> &&callback){
    req_ptr->setBody(fast_write.write(json));
    client_ptr->sendRequest(req_ptr,[=](drogon::ReqResult req_res,const drogon::HttpResponsePtr &res_ptr){
        if(res_ptr->getContentType()!=drogon::CT_APPLICATION_JSON){
            callback("cannot get json from "+url,-1);
        }
        else{
            callback(*(res_ptr->getJsonObject()),1);
        }
    },0);
}


Bank_client_cb::Bank_client_cb(std::string url) : client(url)
{
}
int Bank_client_cb::change_money(int from_id, int to_id, int money,std::function<void (Json::Value json,int res)>&&callback)
{
    Json::Value req_json, reponse_json;
    Json::Value req_data;
    req_data["trade_from"] = from_id;
    req_data["trade_to"] = to_id;
    req_data["trade_money"] = money;
    req_json["content"].append(req_data);
    client.request(req_json,[=](Json::Value json, int res){
        callback(json,res);
    });
}

Tool_client_cb::Tool_client_cb(std::string url) : client(url)
{
}


int Tool_client_cb::op_tool(int user_id, int tool_id, int num, std::string operation,std::function<void (Json::Value json,int res)>&callback){
    Json::Value req_json, reponse_json;
    req_json["operation"] = operation;
    req_json["user_id"] = std::to_string(user_id);
    req_json["prop_id"] = std::to_string(tool_id);
    req_json["num"] = std::to_string(num);
    client.request(req_json,[=](Json::Value json, int res){
        callback(json,res);
    });
}
int Tool_client_cb::remove_tool(int user_id, int tool_id, int num,std::function<void (Json::Value json,int res)>&&callback){
    return op_tool(user_id,tool_id,num, "minus",callback);
}
int Tool_client_cb::add_tool(int user_id, int tool_id, int num,std::function<void (Json::Value json,int res)>&&callback){
    return op_tool(user_id,tool_id,num, "add",callback);
}

