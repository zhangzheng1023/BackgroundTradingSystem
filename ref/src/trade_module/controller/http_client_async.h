#ifndef __HTTP_CLIENT_ASYNC_H
#define __HTTP_CLIENT_ASYNC_H
#include <drogon/drogon.h>

class Http_client{
public:
    Http_client(std::string &ip, uint16_t port);
    Http_client(std::string host_string);
    int request(Json::Value data, double timeout = 0.0);
    drogon::HttpClientPtr client_ptr;
    drogon::HttpRequestPtr req_ptr;
    void callback(drogon::ReqResult req_res,const drogon::HttpResponsePtr req_ptr);
private:

};


#endif