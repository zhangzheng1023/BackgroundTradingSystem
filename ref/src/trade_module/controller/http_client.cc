#include "http_client.h"
#include <unistd.h>
Http_client::Http_client()
{
    handle = curl_easy_init();
}
Http_client::Http_client(std::string _url)
{
    url = _url;
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
}
int Http_client::request(Json::Value req_data, Json::Value &reponse_data, double timeout)
{
    std::string req_str = json_to_str.write(req_data);
    LOG_INFO << url+" "+req_str;
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, req_str.data());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, req_str.size());
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writecb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&reponse_data);
        return curl_easy_perform(handle);
    }
    else
    {
        LOG_ERROR << "http handler is null";
        return -1;
    }
}

Http_client::~Http_client()
{
    curl_easy_cleanup(handle);
}
void test_json(Json::Value *json)
{
    const char *json_str = "{\"data\":\"hello world\"}";
    Json::Reader reader;
    reader.parse(json_str, *json);
}

size_t writecb(void *data, size_t size, size_t nmemb, void *json)
{
    size_t real_size = size * nmemb;
    Json::Value *_json = ((Json::Value *)json);
    Json::Reader reader;
    if(!reader.parse((char *)data, *_json)){
         LOG_INFO << "recv http reponse not json: "<<(char * )data;
        _json->clear();
    }
    return real_size;
}

Bank_client::Bank_client(std::string url) : client(url)
{
}
int Bank_client::change_money(int from_id, int to_id, int money)
{
    Json::Value req_json, reponse_json;
    Json::Value req_data;
    req_data["trade_from"] = from_id;
    req_data["trade_to"] = to_id;
    req_data["trade_money"] = money;
    req_json["content"].append(req_data);
    int res = client.request(req_json, reponse_json);
    if (res == CURLE_OK)
    {
        if(reponse_json.isNull()){
            return -1;
        }
        if (reponse_json["errno"] == 0)
        {
            return 1;
        }
        else
        {
            LOG_ERROR << "change money fail " << reponse_json["errmsg"].asString();
            return -1;
        }
    }
    else
    {
        LOG_ERROR << "bank service fail :" << res;
        return -1;
    }
}

Tool_client::Tool_client(std::string url) : client(url)
{
}


int Tool_client::op_tool(int user_id, int tool_id, int num, std::string operation){
    Json::Value req_json, reponse_json;
    req_json["operation"] = operation;
    req_json["user_id"] = std::to_string(user_id);
    req_json["prop_id"] = std::to_string(tool_id);
    req_json["num"] = std::to_string(num);
    int res = client.request(req_json,reponse_json);
    if(res==CURLE_OK){
        if(reponse_json.isNull()){
            return -1;
        }
        if(reponse_json["status"]=="true"){
            return 1;
        }
        else{
            LOG_ERROR<< operation<<" tool fail "\
                     << " user id: "<<user_id\
                     << " tool_id: "<<tool_id\
                     <<" num:" <<num;
            return -1;
        }
    }
    else{
        LOG_ERROR << "tool manage connect fail :"<< res;
        return -2;
    }
}
int Tool_client::remove_tool(int user_id, int tool_id, int num){
    return op_tool(user_id,tool_id,num, "minus");
}
int Tool_client::add_tool(int user_id, int tool_id, int num){
    return op_tool(user_id,tool_id,num, "add");
}

/*
int main(int argc, char **argv)
{
    curl_global_init(CURL_GLOBAL_ALL);
    Http_client hc("http://www.baidu.com");
    Json::Value request_json, reponse_json;
    //test_json(&reponse_json);

    int res = hc.request(request_json, reponse_json);
    if (res != CURLE_OK)
    {
        LOG_DEBUG << "get fail  " << res;
    }
    LOG_DEBUG << reponse_json["data"].asString() << " end";
    curl_global_cleanup();
}
*/