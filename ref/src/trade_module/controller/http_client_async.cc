#include "http_client_async.h"
#include <unistd.h>
#include <curl/curl.h>
Http_client::Http_client(std::string host_string)
{
    client_ptr = drogon::HttpClient::newHttpClient(host_string);
    req_ptr = drogon::HttpRequest::newHttpRequest();
    req_ptr->setMethod(drogon::Get);
}

Http_client::Http_client(std::string &ip, uint16_t port)
{
    client_ptr = drogon::HttpClient::newHttpClient(ip, port);
    req_ptr = drogon::HttpRequest::newHttpRequest();
    req_ptr->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    req_ptr->setMethod(drogon::Post);
}
int Http_client::request(Json::Value data, double timeout)
{
    //client_ptr->sendRequest(req_ptr, callback,10);
    std::pair<drogon::ReqResult, drogon::HttpResponsePtr> reponse_pair = client_ptr->sendRequest(req_ptr);
    LOG_DEBUG << "start";
    auto body = reponse_pair.second->getBody();
    LOG_DEBUG << body;
}

int main(int argc, char **argv)
{
    CURL *handle = curl_easy_init();
    curl_easy_setopt(handle,CURLOPT_URL,argv[1]);
    //curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,)
    CURLcode res = curl_easy_perform(handle);
    if(res == CURLE_OK){
        
    }
    curl_easy_cleanup(handle);

}