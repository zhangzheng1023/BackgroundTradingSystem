from flask import Flask, request
import logging
import json
import database

logging.basicConfig(level=logging.INFO, format="%(asctime)s : %(message)s")
logger = logging.getLogger(__name__)


server = Flask(__name__)
db = database.Database()


@server.route('/register', methods=['POST', 'GET'])
def register():
    data = json.loads(request.json)
    logger.info('接收数据:[%s]' % data)
    id = data['data']['ID']
    name = data['data']['Name']
    password = data['data']['Password']
    try:
        db.addUser(id, name, password)
        data = {'code': 1, 'msg': '注册结果!', 'data': {'0': '账号已存在！', '1': '注册成功！'}}
    except:
        data = {'code': 0, 'msg': '注册结果!', 'data': {'0': '账号已存在！', '1': '注册成功！'}}

    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


@server.route('/login', methods = ['POST', 'GET'])
def login():
    data = json.loads(request.json)
    logger.info('接收数据:[%s]' % data)
    id = data['data']['ID']
    password = data['data']['Password']
    db.selectUser(id)
    if len(db.data) == 0:
        data = {'code': -1, 'msg': '登录失败!', 'data': {"-1": "用户不存在！", "0": "密码错误！"}}
        logger.info('发送数据:[%s]' % data)
        return json.dumps(data)

    user = db.data[0]
    if user[-1] == password:
        db.updateUser(id, 'Status', '在线')
        data = {'ID': user[0], 'Name': user[1], 'Status': '在线', 'Money': user[3], 'Password': user[4]}
        data = {'code': 2, 'msg': '登录成功!', 'data': data}
    else:
        data = {'code': 0, 'msg': '登录失败!', 'data': {"-1": "用户不存在！", "0": "密码错误！"}}
    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


@server.route('/update', methods = ['POST', 'GET'])
def update():
    data = json.loads(request.json)
    logger.info('接收数据:[%s]' % data)
    id = data['data']['ID']
    item = data['data']['Item']
    new_value = data['data']['New_value']
    try:
        db.updateUser(id, item, new_value)
        data = {'code': 3, 'msg': '修改成功!', 'data': None}
    except:
        data = {'code': 0, 'msg': '修改失败!', 'data': None}

    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


@server.route('/select', methods=['POST', 'GET'])
def select():
    data = json.loads(request.json)
    logger.info('接收数据:[%s]' % data)
    id = data['data']['ID']
    db.selectUser(id)

    data = {'code': 4, 'msg': '用户信息!', 'data': db.data}
    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


@server.route('/quit', methods=['POST', 'GET'])
def quit():
    data = json.loads(request.json)
    logger.info('接收数据:[%s]' % data)
    id = data['data']['ID']
    db.updateUser(id, 'Status', '离线')

    data = {'code': 5, 'msg': '退出登录!', 'data': 'NULL'}
    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


# 测试用
@server.route('/hello', methods=['POST', 'GET'])
def hello():
    data = json.loads(request.json)
    print(type(data))
    logger.info('接收数据:[%s]' % data)
    data = {'code': 555, 'msg': '哈哈哈哈!', 'data': 'NULL'}
    logger.info('发送数据:[%s]' % data)
    return json.dumps(data)


if __name__ == '__main__':
    server.run(host='0.0.0.0', port=5000, debug=True)
