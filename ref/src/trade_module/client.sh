# 测试自身的代码
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8888/Trade_service -d '{"func_name":"push_store","argv":{"user_id":2,"tool_id":6,"num":4}}'
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8888/Trade_service -d '{"func_name":"trade","argv":{"from_id":22,"to_id":23,"tool_id":6,"num":1}}'
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8888/Trade_service -d '{"func_name":"get_store_info","argv":{"user_id":22,"tool_id":6}}'
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8888/Trade_service -d '{"func_name":"get_bill_record","argv":{"from_id":22,"to_id":23,"tool_id":6}}'


# test user
curl -XPOST -H "Content-Type:text/plain" http://127.0.0.1:5000/select -d '{"code":4,"msg":"select","data":{"ID":""}}'

# 测试push user_prop的代码
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8088/prop -d '{"operation":"minus","user_id":"22","prop_id":"6","num":"3"}'

# test bank
curl -XPOST -H "Content-Type:application/json" http://127.0.0.1:8086/money/update -d '{"content":[{"trade_from":1003,"trade_to":1004,"trade_money":20}]}'