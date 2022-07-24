#include <drogon/HttpAppFramework.h>
#include "controller/trade_service.h"
#include <stdio.h>
#include <drogon/drogon.h>

void test_json()
{
    Json::Value row, all;
    
    for (int i = 0; i < 10; i++)
    {
        row["prop"] = "123";
        row["prop1"] = 123;
        all.append(row);
    }
    Json::FastWriter fastwrite;
    std::cout<< fastwrite.write(all);
}
int main()
{
    drogon::app().loadConfigFile("../config.json");
    drogon::app().run();
}