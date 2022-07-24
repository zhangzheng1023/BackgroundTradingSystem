#ifndef __HTTP_CLIENT_ASYNC_H
#define __HTTP_CLIENT_ASYNC_H
#include <drogon/drogon.h>

class Http_client_cb{
public:
    Http_client_cb();
    Http_client_cb(std::string &ip, uint16_t port);
    Http_client_cb(std::string host_string);
    void request(Json::Value json,std::function<void (Json::Value data,int res)> &&callback);
private:
    Json::FastWriter fast_write;
    Json::Reader reader;
    drogon::HttpClientPtr client_ptr;
    drogon::HttpRequestPtr req_ptr;
    std::string url;
};

class Bank_client_cb
{
public:
    Bank_client_cb(){};
    Bank_client_cb(std::string url);
    int change_money(int from_id, int to_id, int money,std::function<void (Json::Value json,int res)>&&callback);

private:
    Http_client_cb client;
};

class Tool_client_cb
{
public:
    Tool_client_cb(){};
    Tool_client_cb(std::string url);
    int add_tool(int user_id, int tool_id, int num,std::function<void (Json::Value json,int res)>&&callback);
    int remove_tool(int user_id, int tool_id, int num,std::function<void (Json::Value json,int res)>&&callback);
    
private:
    Http_client_cb client;
    int op_tool(int user_id, int tool_id, int num, std::string operation,std::function<void (Json::Value json,int res)>&callback);
};

#endif
