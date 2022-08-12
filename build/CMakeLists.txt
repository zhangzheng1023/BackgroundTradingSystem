cmake_minimum_required(VERSION 3.2)

PROJECT (BackgroundTradingSystem)

SET(LOG_SRC_LIST ../src/Logger) # log模块源文件路径
SET(TEST_SRC_LIST ../test) # test模块源文件路径

# $(TEST_SRC_LIST)
INCLUDE_DIRECTORIES(${LOG_SRC_LIST})
ADD_EXECUTABLE(test1 ${TEST_SRC_LIST}/LoggerTest.cc ${LOG_SRC_LIST}/logger.cc)