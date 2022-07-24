import operator
from os import environ
import requests
import json
import database 
import threading
import random
import time
file_path = "../config_service.json"
with open(file_path,"r") as f:
        config_service=json.load(f)
config_service=config_service["sandbox"]
db_config = config_service["db_client"]
user_service=config_service['user_client']
tool_service = config_service["tool_client"]
bank_service = config_service["bank_client"]
trade_service = config_service["trade_client"]
def get_service(config,data):
    url = 'http://'+config['ip']+':'+config['port']+config['service']
    print(url)
    print(str(data))
    #res = requests.post(url,data=json.dumps(data)).json() #发送了普通得data
    res = requests.post(url,json=data).json() #发送了json

    return res
def push_store():
    data={}
    data['func_name']= "push_store"
    argv={'user_id':2,"tool_id":6,"num":4}
    data['argv'] = argv
    res_json = get_service(trade_service,data)
class Db_client:
    def __init__(self) -> None:
        self.db_local=database.Database(db_config["host"],db_config["port"],db_config["user"],db_config["password"],db_config["db_name"])
    def get_money(self,user_id):
        sql = "select money from User where ID="+str(user_id)
        self.db_local.databaseOperation(sql)
        return self.db_local.data[0][0]

    def get_all_user(self):
        sql = "select ID from User"
        self.db_local.databaseOperation(sql)
        return self.db_local.data

    def get_user_tool_num(self,user_id,tool_id):
        sql = "select num from user_prop where user_id="+str(user_id)+" and tool_id="+ str(tool_id)
        self.db_local.databaseOperation(sql)
        if(len(self.db_local.data)==0 or len(self.db_local.data[0])==0):
            return 0
        return self.db_local.data[0][0]

    def get_price(self,user_id,tool_id):
        sql = "select price from on_sale_t where user_id="+str(user_id)+" and tool_id="+ str(tool_id)
        self.db_local.databaseOperation(sql)
        return self.db_local.data[0][0]
db_global = Db_client()
def get_random_sale():
    sql = "select user_id,tool_id,remain_num,price from on_sale_t"
    db_global.db_local.databaseOperation(sql)
    row_num = len(db_global.db_local.data)
    if(row_num==0):
        return None
    row = random.randint(0,row_num-1)
    sale_info = {}
    sale_info["user_id"] = db_global.db_local.data[row][0]
    sale_info["tool_id"] = db_global.db_local.data[row][1]
    sale_info["remain_num"] = db_global.db_local.data[row][2]
    sale_info["price"] = db_global.db_local.data[row][3]
    return sale_info


test_data = {
    "from_id_money":123,
    "to_id_money":123,
    "from_tool_num":123,
    "to_tool_num":123
}
def test_trade(from_id,to_id,tool_id,num,thread_index):
    db_thread = Db_client()
    old_data = {
        "from_id_money":db_thread.get_money(from_id),
        "to_id_money":db_thread.get_money(to_id),
        "from_tool_num":db_thread.get_user_tool_num(from_id,tool_id),
        "to_tool_num":db_thread.get_user_tool_num(to_id,tool_id)
    }
    price = db_thread.get_price(from_id,tool_id)
    test_data = {
        "from_id_money":old_data["from_id_money"]+price*num,
        "to_id_money":old_data["to_id_money"]-price*num,
        "from_tool_num":old_data["from_tool_num"],
        "to_tool_num":old_data["to_tool_num"]+num
    }
    data = {}
    data["func_name"] = "trade"
    argv = {}
    argv["from_id"] = from_id
    argv["to_id"] = to_id    
    argv["tool_id"]= tool_id
    argv["num"] = num
    data["argv"] = argv
    start_time = time.time()
    res_json = get_service(trade_service,data)
    stop_time = time.time()
    new_data = {
        "from_id_money":db_thread.get_money(from_id),
        "to_id_money":db_thread.get_money(to_id),
        "from_tool_num":db_thread.get_user_tool_num(from_id,tool_id),
        "to_tool_num":db_thread.get_user_tool_num(to_id,tool_id)
    }

    if(res_json["status"] == "fail"):
        if(operator.eq(old_data,new_data)):
            threads[thread_index]["trade_status"] = False
            threads[thread_index]["verify"] = True
        else:
            threads[thread_index]["trade_status"] = False
            threads[thread_index]["verify"] = True
    else:
        if(operator.eq(test_data,new_data)):
            threads[thread_index]["trade_status"] = True
            threads[thread_index]["verify"] = True
        else:
            threads[thread_index]["trade_status"] = True
            threads[thread_index]["verify"] = False

    threads[thread_index]["args"]={
        "from_id":from_id,
        "to_id":to_id,
        "num":num,
        "tool_id":tool_id
    }
    threads[thread_index]["old_data"]=old_data
    threads[thread_index]["test_data"]=test_data
    threads[thread_index]["new_data"]=new_data
    threads[thread_index]["start_time"] = start_time
    threads[thread_index]["stop_time"] = stop_time

threads = []
struct = {
    "thread":"thread",
    "args":"",
    "trade_status":"",
    "verify":"",
    "old_data":{
        "from_id_money":"",
        "to_id_money":"",
        "from_tool_num":"",
        "to_tool_num":""
    },
    "test_data":"",
    "new_data":"",
    "start_time":1,
    "stop_time":2
}

# 相同得tool_id from_id,多个不同to_id，所有的num
def multi_pthread_init(thread_num):
    sale_info = get_random_sale()
    from_id = sale_info["user_id"]
    tool_id = sale_info["tool_id"]
    num = sale_info["remain_num"]
    all_user_id = db_global.get_all_user()
    for i in range(thread_num):
        if(all_user_id[i] == from_id):
            continue
        thread_struct = {}
        thread_struct["thread"] =  threading.Thread(target=test_trade,args=(from_id,all_user_id[i][0],tool_id,num,i))
        threads.append(thread_struct)

def multi_pthread_start(thread_num):
    multi_pthread_init(thread_num)
    for t in threads:
        t["thread"].start()
    for t in threads:
        t["thread"].join()

def analyse(test_data_record = threads):
    all_test_num = len(test_data_record)
    trade_fail_num = 0
    trade_ok_num = 0
    trade_fail_verify_ok_num = 0
    trade_fail_verify_fail_num = 0
    trade_ok_verify_ok_num=0
    trade_ok_verify_fail_num=0
    max_time = -1
    time_list = []
    average_time = 0
    middle_time = 0
    for thread in test_data_record:
        del thread["thread"]
        interval_time = thread["stop_time"]-thread["start_time"]
        time_list.append(interval_time)
        if(thread["trade_status"]==True):
            trade_ok_num+=1
            if(thread["verify"]==True):
                trade_ok_verify_ok_num+=1
            else:
                trade_ok_verify_fail_num+=1
        else:
            trade_fail_num+=1
            if(thread["verify"]==True):
                trade_fail_verify_ok_num+=1
            else:
                trade_fail_verify_fail_num+=1
    time_list.sort()
    max_time = time_list[len(time_list)-1]
    average_time = sum(time_list)/len(time_list)
    middle_time = time_list[int(len(time_list)/2)]
    min_time = time_list[0]
    result = {}
    result["all_test_num"] = all_test_num
    result["trade_ok_num"] = trade_ok_num
    result["trade_fail_num"] = trade_fail_num
    result["trade_fail_verify_fail_num"] = trade_fail_verify_fail_num
    result["trade_fail_verify_ok_num"] = trade_fail_verify_ok_num
    result["trade_ok_verify_ok_num"] = trade_ok_verify_ok_num
    result["trade_ok_verify_fail_num"] = trade_ok_verify_fail_num
    result["ratio_trade_ok"] = trade_ok_num/all_test_num
    result["ratio_verify_ok"] = (trade_ok_verify_fail_num+trade_fail_verify_fail_num)/all_test_num
    result["max_time"] = max_time
    result["average_time"] = average_time
    result["middle_time"] = middle_time
    result["min_time"] = min_time
    return result

def save(record,result):
    data = {
        "detail":record,
        "sum":result
    }
    print(json.dumps(result,sort_keys=True,indent=4,separators=(',',':')))
    with open("data.txt","w") as f:
        json.dump(data,f,sort_keys=True,indent=4,separators=(',',':'))

def test():
    lists = [1,2,3,4]
    print(sum(lists))
    a=1
    b=2
    print(a/b)
    print(time.time())
    print(db_global.get_money(1003))
    print(get_random_sale())
    print(time.time())
    db = Db_client()
    use_list = db.get_all_user()
    print(use_list)
    for i in range(len(use_list)):
        print(db.get_money(use_list[i][0]))
    test_trade(1003,1001,1320,7,1)
    exit(0)
if __name__ =="__main__":
    #test()
    multi_pthread_start(50)
    result = analyse(threads)
    save(threads,result)

    
