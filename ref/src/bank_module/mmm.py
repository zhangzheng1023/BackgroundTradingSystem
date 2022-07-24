import time
import json
import pymysql
import requests
import logging
from flask import Flask, request, jsonify, make_response

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler("debug.log"),
        logging.StreamHandler()
    ]
)

# connect to db
dbhost = '1.12.243.185'
dbuser = 'root'
dbpass = '' 
dbname = 'YinLi'
db = pymysql.connect(host=dbhost,user=dbuser,password=dbpass,database=dbname)
cursor = db.cursor()


# user_module
user_module_ip = '127.0.0.1'
user_module_port = '5000'
user_module_quer = "http://{}:{}/select".format(user_module_ip, user_module_port)
user_module_up = "http://{}:{}/update".format(user_module_ip, user_module_port)

app = Flask(__name__)

@app.route("/money/update", methods=['GET', 'POST'])
def update():
    r = json.loads(request.get_data())
    data = r['content']
    buyer_id = data[0]['trade_from']
    seller_id = data[0]['trade_to']
    money = data[0]['trade_money']

    sql_uw_update = " INSERT INTO user_wallet \
                   (user_id, change_log, before_money, after_money, trans_time)\
                   VALUES(%d, %d, %d, %d, %d)"
    sql_cf_update = "insert into capital_flow \
        (buyer_id, seller_id, money) \
        values(%d, %d, %d)" % (buyer_id, seller_id, money)
    try:
        # step 1: sync with user_info
        #cursor.execute(sql_buyer_sync)
        #before_money_buyer = cursor.fetchone()
        #before_money_buyer = before_money_buyer[0]
        
        info1 = {'code': 4, 'msg': '查询用户!', 'data': {'ID': buyer_id}}

        info2 = {'code': 4, 'msg': '查询用户!', 'data': {'ID': seller_id}}

        r1 = requests.post(user_module_quer, json=json.dumps(info1))       
        before_money_buyer = r1.json()["data"][0][3]

        if before_money_buyer < money:
            response = make_response(jsonify(errno=1, errmsg= "insufficient balance"), 400)
            return response
        #cursor.execute(sql_seller_sync)
        #before_money_seller = cursor.fetchone()
        #before_money_seller = before_money_seller[0]
        
        r2 = requests.post(user_module_quer, json=json.dumps(info2))
        before_money_seller = r2.json()["data"][0][3]
       
        after_money_buyer = before_money_buyer - money
        after_money_seller = before_money_seller + money
        # step 2: update table user_wallet
        sql_insert_buyer = sql_uw_update % (buyer_id, money * (-1), before_money_buyer, after_money_buyer, int(time.time()))
        sql_insert_seller= sql_uw_update % (seller_id, money, before_money_seller, after_money_seller, int(time.time()))
        cursor.execute(sql_insert_buyer)
        cursor.execute(sql_insert_seller)

        # step 3: update table capital_flow
        cursor.execute(sql_cf_update)
        # step 4: update table user_info
        
        """
        sql_buyer_update = "update User set Money = %d where ID = %d" % (after_money_buyer, buyer_id)
        sql_seller_update = "update User set Money = %d where ID = %d " % (after_money_seller, seller_id)
        cursor.execute(sql_buyer_update)
        cursor.execute(sql_seller_update)
        """

        info3 = {'code': 3, 'msg': '修改用户信息!', 'data': {'ID': buyer_id, 'Item': "Money", 'Value': after_money_buyer}}

        info4 = {'code': 3, 'msg': '修改用户信息!', 'data': {'ID': seller_id, 'Item': "Money", 'Value': after_money_seller}}

        r3 = requests.post(user_module_up, json=json.dumps(info3))
        r4 = requests.post(user_module_up, json=json.dumps(info4))
        if r3.json()["code"] != 3 or r4.json()["code"] != 3:
            response = make_response(jsonify(errno=1, errmsg="table User update error"), 500)
            db.rollback()
            return response
        db.commit()
    #except Exception,err:
    #    response = make_response(jsonify(errno=1, errmsg=err), 500)
    #    return response
    except:
        db.rollback()
        response = make_response(jsonify(errno=2, errmsg="module mmm update error"), 500)
        return response
    else:
        response = make_response(jsonify(errno=0, errmsg="SUCCESS"), 200)
        return response


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8086, debug=True)
