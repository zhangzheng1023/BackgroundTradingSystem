#include "../controller/trade_db_client.h"
#include <iostream>
#include <fstream>
int main(int argc, char **argv){
    Db_client *db_client;
    Json::Reader reader;
    Json::Value root;
    std::ifstream in;
    in.open(argv[1],ios::binary);
    reader.parse(in,root);
    Json::Value &tool_config = root["tool_client"];
    Json::Value &bank_config = root["bank_client"];
    Json::Value &db_config = root["db_client"];
    db_client = new Db_client(db_config);
    //db_client->lock_trade_more_num(6,2);
    db_client->unlock_trade(6);
}