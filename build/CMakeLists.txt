cmake_minimum_required(VERSION 3.2)

PROJECT (BackgroundTradingSystem)

SET(DATABASE_SRC_LIST ../src/DataBase) # DataBase模块的源文件路径
SET(LOG_SRC_LIST ../src/Logger) # log模块源文件路径
SET(TEST_SRC_LIST ../test) # test模块源文件路径
SET(MYSQL_LIST /usr/lib/x86_64-linux-gnu)
SET(WEB_SRC_LIST ../src/WebServer)

SET(Libs mysqlclient)

# $(TEST_SRC_LIST)
INCLUDE_DIRECTORIES(${LOG_SRC_LIST})
INCLUDE_DIRECTORIES(${DATABASE_SRC_LIST})
INCLUDE_DIRECTORIES(${WEB_SRC_LIST})

# LINK_DIRECTORIES(MYSQL_LIST) 

# ADD_EXECUTABLE(logTest ${TEST_SRC_LIST}/LoggerTest.cc ${LOG_SRC_LIST}/logger.cc)
# ADD_EXECUTABLE(dataBaseTest ${TEST_SRC_LIST}/DataBase.cc ${DATABASE_SRC_LIST}/mysql_conn.cc)
ADD_EXECUTABLE(webTest ${WEB_SRC_LIST}/main.cc ${WEB_SRC_LIST}/httpConn.cc ${WEB_SRC_LIST}/locker.cc)

# target_link_libraries(dataBaseTest PRIVATE ${Libs})