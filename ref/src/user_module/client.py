import requests
import json
import logging
from prettytable import PrettyTable

logging.basicConfig(level=logging.INFO, format="%(asctime)s : %(message)s")
logger = logging.getLogger(__name__)

class Client:
    def __init__(self):
        self.ip = '10.0.0.17'
        self.port = '5000'
        c = input('请选择注册（R）或者 登录（L）:')
        if c == 'R':
            id = input("请输入账号：")
            name = input('请输入昵称：')
            password = input("请输入密码：")
            self.register(id, name, password)
        elif c=='L':
            id = input("请输入账号：")
            password = input("请输入密码：")
            self.login(id, password)
        else:
            print('输入错误！')
            self.__init__()

    def register(self,id, name, password):
        url = 'http://%s:%s/register' % (self.ip, self.port)
        data = {'ID': id, 'Name': name, 'Password': password}
        data = {'code': 1, 'msg': '请求注册!', 'data': data}
        logger.info('发送数据：[%s]' % data)
        res = requests.post(url, json=json.dumps(data))
        data = res.json()
        logger.info('接收数据：[%s]' % data)

        if data['code'] == 1:
            print('********' + data['data']['1'] + '********')
            print('********新用户登录********')
            id = input("请输入账号：")
            password = input("请输入密码：")
            self.login(id, password)
        elif data['code'] == 0:
            print('********' + data['data']['0'] + '********')
            print('请直接登录！')
            id = input("请输入账号：")
            password = input("请输入密码：")
            self.login(id, password)



    def login(self, id, password):
        url = 'http://%s:%s/login' % (self.ip, self.port)
        data = {'ID': id, 'Password': password}
        data = {'code': 2, 'msg': '登录请求', 'data': data}
        logger.info('发送数据：[%s]' % data)
        res = requests.post(url, json=json.dumps(data))
        data = res.json()
        logger.info('接收数据：[%s]' % data)

        if data['code'] == 2 :
            print('\n\n********' + data['msg'] + '********')
            self.user_info = data['data']
            print("用户账号：", self.user_info['ID'])
            print("用户昵称：", self.user_info['Name'])
            print("用户状态：", self.user_info['Status'])
            print("用户资产：", self.user_info['Money'])
        elif data['code'] == 0:
            print('\n\n********' + data['msg'] +'原因：'+data['data']['0']+ '********')
            id = input("请重新输入账号：")
            password = input("请重新输入密码：")
            self.login(id, password)
        elif data['code'] == -1:
            print('\n\n********' + data['msg'] + '原因：' + data['data']['-1'] + '********')
            print('登录前请先注册！')
            id = input("请输入账号：")
            name = input('请输入昵称：')
            password = input("请输入密码：")
            self.register(id, name, password)


    def update(self, id, item, new_value):
        url = 'http://%s:%s/update' % (self.ip, self.port)
        data = {'ID': id, 'Item': item, 'New_value': new_value}
        data = {'code': 3, 'msg': '修改用户信息!', 'data': data}
        logger.info('发送数据：[%s]' % data)
        res = requests.post(url, json=json.dumps(data))
        data = res.json()
        logger.info('接收数据：[%s]' % data)
        print('\n\n********' + data['msg'] + '********')

    def select(self, id=""):
        url = 'http://%s:%s/select' % (self.ip, self.port)
        data = {'ID': id}
        data = {'code': 4, 'msg': '查询用户!', 'data': data}
        logger.info('发送数据：[%s]' % data)
        res = requests.post(url, json=json.dumps(data))
        data = res.json()
        logger.info('接收数据：[%s]' % data)

        users = data['data']
        table = PrettyTable(["账号", '昵称', '状态', '资产', '密码'])
        for user in users:
            table.add_row([user[0], user[1], user[2], user[3], '*****'])
        print(table.get_string(title = '玩家信息'))
        print('【当前用户】 账号：', self.user_info['ID'], ' 昵称：',self.user_info['Name'])
        print('-----------------------------------------\n')

    def quit(self):
        url = 'http://%s:%s/quit' % (self.ip, self.port)
        data = {'ID': self.user_info['ID']}
        data = {'code': 5, 'msg': '退出登录!', 'data': data}
        logger.info('发送数据：[%s]' % data)
        res = requests.post(url, json=json.dumps(data))
        data = res.json()
        logger.info('接收数据：[%s]' % data)
        print('********' + data['msg'] + '********')

if __name__ =='__main__':
    user = Client()
    while True:
        print('\n服务类型：查询用户信息（S）  修改用户信息（U）  退出登录（Q）')
        c = input('请选择服务类型：')
        if c == 'S':
            id = input('请选择待查询用户（id）或者 所有用户（*） ：')
            id = "" if id == '*' else id
            user.select(id)
        elif c == 'U':
            id = input("请输入用户账号：")
            item = input('请选择修改类型 昵称(Name) 密码（Password）:')
            new_value = input('将其修改为：')
            user.update(id, item, new_value)
        elif c == 'Q':
            user.quit()
            break
        else:
            print('输入错误！');




