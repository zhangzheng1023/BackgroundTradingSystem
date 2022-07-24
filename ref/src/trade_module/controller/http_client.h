#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H
#include <curl/curl.h>
#include <drogon/drogon.h>

class Http_client
{
public:
    Http_client(std::string url);
    Http_client();
    ~Http_client();
    int request(Json::Value req_data, Json::Value &reponse_data, double timeout = 0.0);

private:
    CURL *handle;
    Json::Reader str_to_json;
    Json::FastWriter json_to_str;
    std::string url;
};
size_t writecb(void *data, size_t size, size_t nmemb, void *json);

class Bank_client
{
public:
    Bank_client(){};
    Bank_client(std::string url);
    int change_money(int from_id, int to_id, int money);

private:
    Http_client client;
};

class Tool_client
{
public:
    Tool_client(){};
    Tool_client(std::string url);
    int add_tool(int user_id, int tool_id, int num);
    int remove_tool(int user_id, int tool_id, int num);
    
private:
    Http_client client;
    int op_tool(int user_id, int tool_id, int num, std::string operation);
};

#endif