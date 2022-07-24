import requests

mmm_url = 'http://127.0.0.1:8086/money/update'

request_data = {
    "content":[
    {"trade_id": 1,
    "trade_from": 1,
    "trade_to": 2,
    "trade_money": 2000
    }]
}

r = requests.get(request_data)
print(r)


