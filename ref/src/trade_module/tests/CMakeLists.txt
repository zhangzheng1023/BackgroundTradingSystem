cmake_minimum_required(VERSION 3.5)

project(test_mysql)
#add_subdirectory(third_party/drogon)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
# scl enable gcc-toolset-10 bash centos8升级gcc到版本10, ubuntu可以直接安装gcc-10 g++-10
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

find_package(Drogon CONFIG REQUIRED)
#execute_process(COMMAND mysql_path=$(mysql_config --libs | awk -F '[ L]' '{print $2}'))
link_directories("/usr/lib64/mysql")
set(libs
    Drogon::Drogon
    curl
    mysqlclient
    )
set(src_lists
    test_mysql_manager.cpp
    ../controller/mysql_base.cc
    ../controller/mysql_base.h
    ../controller/trade_db_client.cc
    ../controller/trade_db_client.h)
set(test_trade_operate
    teat_trade_operate.cpp
    ../controller/trade_operate.h
    ../controller/trade_operate.cc)

add_executable(test_mysql ${src_lists})
add_executable(test_trade_operate ${src_lists})

target_link_libraries(test_mysql PRIVATE ${libs})
target_link_libraries(test_trade_operate PRIVATE ${libs})
