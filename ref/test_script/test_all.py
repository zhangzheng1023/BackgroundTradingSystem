
import requests
import json
import random
import uuid

user_config=None
trade_config = None
tool_config = None
bank_config = None
database_config = None
def load_config(file_path):
    config = None
    with open(file_path,"r") as f:
        config = json.load(f)
    user_config = config["user_service"]
    tool_config = config["tool_service"]
    bank_config = config["bank_service"]
    trade_config = config["trade_service"]
    database_config = config["database"]

def random_id():
    uuid.uuid4
    return random.randint(0,10000)
def random_str(string_length=10):
    """Returns a random string of length string_length."""
    random_str = str(uuid.uuid4()) # Convert UUID format to a Python string.
    random_str = random_str.replace("-","") # Remove the UUID '-'.
    return random[0:string_length] # Return the random string.

def init_user(num = 100):
    url = 'http://'+user_config['ip']+':'+user_config['port']+user_config['register']
    for i in range(100):
        data = {'ID': str(i), 'Name': random_str(), 'Password': random_str}
        data = {'code': 1, 'msg': '请求注册!', 'data': data}
        requests.post(url,json = json.dumps(data))



    

if __name__ == '__main__':
    load_config("./service_config.json")
    init_user()