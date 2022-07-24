import pymysql

class Database:
    def __init__(self, host='10.0.0.17', port=3306, user='root', password='', database='YinLi', charset='utf8'):
        self.host = host
        self.port = port
        self.user = user
        self.password = password
        self.database = database
        self.charset = charset
        self.data = ()

    def databaseOperation(self, sql):
        self.data = ()
        database = pymysql.connect(host=self.host, port=self.port, user=self.user, password=self.password, database=self.database, charset=self.charset)
        cursor = database.cursor()
        try:
            cursor.execute(sql)
            database.commit()
        except:
            database.rollback()
        self.data = cursor.fetchall()
        cursor.close()
        database.close()

    def createTable(self):
        sql = "CREATE TABLE User(ID CHAR(10) PRIMARY KEY,Name TEXT, Status ENUM('在线','离线'),Money INTEGER,Password CHAR(10))";
        self.databaseOperation(sql);

    def addUser(self, id, name, password, status='离线', money='30000'):
        sql = "INSERT INTO User VALUES('%s', '%s', '%s', '%s', '%s')" % (id, name, status, money, password)
        self.databaseOperation(sql)

    def deleteUser(self, id):
        sql = "DELETE FROM User WHERE ID='%s'" % id
        self.databaseOperation(sql)

    def selectUser(self, id=""):
        if id=='':
            sql = "SELECT * FROM User"
        else:
             sql = "SELECT * FROM User WHERE ID='%s'" % id
        self.databaseOperation(sql)

    def updateUser(self, id, item, new_value):
        sql = "UPDATE User SET " + item + " = '%s' WHERE ID = '%s'" % (new_value, id)
        self.databaseOperation(sql)

if __name__ == '__main__':
    db = Database()
    # db.createTable()
    # db.addUser('1001', '张红毅', '123')
    # db.addUser('1002', '高峰', '456')
    #db.addUser('1003', '陈勇', '456')
    #db.updateUser('1003', 'Password', '789')
    #db.deleteUser('1003')
    db.selectUser('1001')
    print(type(db.data),db.data[0][-1])
