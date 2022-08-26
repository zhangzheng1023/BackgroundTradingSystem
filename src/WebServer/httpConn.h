#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "sys/epoll.h"
#include "locker.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/uio.h>

class HttpConn {
public:

    static int m_epollfd; // 所有的socket上的事件都被注册到同一个epoll对象
    static int m_user_count;  // 统计用户数量

    HttpConn() {}
    ~HttpConn() {}

    void process(); // 处理客户端的请求
    void init(int sockfd, const sockaddr_in &addr);  // 初始化新接收到的连接
    void closeConn();  // 关闭连接

    bool read(); // 非阻塞的读
    bool write(); // 非阻塞的写

private:
    int m_sockfd; // 该HTTP连接的socket
    sockaddr_in m_address; // 通信的socket地址

};


#endif