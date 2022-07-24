cmake_minimum_required(VERSION 3.5)


add_subdirectory(tests)
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
set(server_lists
    server.cc
    controller/trade_service.h
    controller/trade_service.cc
    controller/trade_operate.h
    controller/trade_operate.cc
    controller/http_client.cc
    controller/http_client.h
    controller/mysql_base.cc
    controller/mysql_base.h
    controller/trade_db_client.cc
    controller/trade_db_client.h
    controller/http_client_cb.h
    controller/http_client_cb.cc)

add_executable(server ${server_lists})

target_link_libraries(server PRIVATE ${libs})


