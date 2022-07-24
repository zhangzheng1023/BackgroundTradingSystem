
#include "../controller/trade_operate.h"

int main(int argc, char **argv){
    Trade_operate trade(argv[1]);
    trade.pop_store(22,6,1);
    cout<<"ok";
}