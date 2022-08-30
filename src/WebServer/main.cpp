#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<signal.h>
#include"locker.h"
#include"threadpool.h"
#include"http_conn.h"

#define MAX_FD 65535
#define MAX_EVENT_NUMBER 10000//监听最大的数量

//若一端断开连接，会发送一个信号。该函数专门做信号处理
//void(handler)int是回调函数，即处理信号的函数
//添加信号捕捉
void addsig(int sig,void(handler)(int)){
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler=handler;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}

//添加文件描述符到Epoll中
extern void addfd(int epollfd,int fd,bool one_shot);
//从epoll中删除文件描述符
extern void removefd(int epollfd,int fd);
//修改epoll中的文件描述符
extern void modfd(int epollfd,int fd,int ev);

int main(int argc,char *argv[]){
    if(argc<=1){
        //argvv[0]:程序名
        std::cout<<"In "<<argv[0]<<", port number is missed!"<<std::endl;
        exit(-1);
    }
    int port=atoi(argv[1]);
    //对sigpie信号进行处理.SIG_IGN即捕捉到信号啥也不做
    addsig(SIGPIPE,SIG_IGN);

    //初始化线程池，本程序的任务就是一个连接任务
    threadpool<http_conn>* pool=nullptr;
    try{
        pool=new threadpool<http_conn>;
    }catch(...){
        exit(-1);
    }

    //创建一个数组用于保存所有客户端信息，用于记录所有的任务请求。
    http_conn *users=new http_conn[MAX_FD];

    //创建监听套接字
    /*第一个参数指明通信域，PF_INET为IPV4。
      第二个参数指明通信类型，STREAM为TCP，DGRAM为UDP。
      第三个指明通信协议，为0则根据前面的自动设置
    */
    int listenfd=socket(PF_INET,SOCK_STREAM,0);
    //设置端口复用
    int reuse=1;
    /*setsockopt为设置socket状态
      第二个参数指定网络层，一般为存取socket层
      第三个参数是设置的具体状态，可重用是指端口释放后立即可用（不设置就是释放两分钟后才可重用
      第四个参数是第三个参数的长度
      返回值：成功返回0，失败返回-1
    */
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    //绑定套接字与端口
    struct sockaddr_in address;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    bind(listenfd,(struct sockaddr*)&address,sizeof(address));
    /*监听。完全连接队列（处于established状态）的大小可以通过listen函数的第二个参数控制
      listen函数不会阻塞，在它设置完之后就会返回。监听的实质由内核执行。
      listen函数只是将主动连接变成被动连接
    */
    listen(listenfd,5);

    //多路复用
    //创建事件数组。事件：读/写、中断、错误等等。event会保存事件以及触发事件的文件描述符
    epoll_event events[MAX_EVENT_NUMBER];
    //创建epoll对象。该函数的size可以被忽略，只要大于0就OK（性能更新）。返回值>0返回的是创建的句柄，-1为失败
    int epollfd=epoll_create(5);
    //将监听的文件描述符添加到epoll中
    //主线程的监听不需要设置为oneshot，因为不会给它分配别的线程，也不会重新注册oneshot
    addfd(epollfd,listenfd,false);
    //因为工作线程处理数据之后要向epoll中注册写事件，所以需要获得epoll对象的FD
    http_conn::m_epollfd=epollfd;
    
    //开始工作，接收客户端请求
    while(true){
        int num=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if(num<0&&errno!=EINTR){
          std::cout<<"epoll failure"<<std::endl;
          break;
        }
        //循环遍历事件数组
        for(int i=0;i<num;++i){
            int sockfd=events[i].data.fd;
            //有客户端连接进来
            if(sockfd==listenfd){
                struct sockaddr_in client_address;
                socklen_t client_addr_len=sizeof(client_address);
                int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addr_len);
                //客户端连接超出限制，可以给客户端回写一个服务器访问繁忙的信息
                if(http_conn::m_user_count>=MAX_FD){
                    close(connfd);
                    continue;
                }
                //将新的客户数据初始化，放到客户数组中。（有新的任务请求）
                users[connfd].init(connfd,client_address);
            }
            //对方异常断开或错误发生
            else if(events[i].events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
                //关闭连接
                users[sockfd].close_conn();
            }
            //有读的事件发生，先读完再交由工作线程处理
            else if(events[i].events&EPOLLIN){
                if(users[sockfd].read()){
                    //读完之后将任务加在请求队列中
                    pool->append(users+sockfd);
                }
                else users[sockfd].close_conn();
            }
            else if(events[i].events&EPOLLOUT){
                //一次性把数据写完
                if(!users[sockfd].write())users[sockfd].close_conn();
            }
        }
    }
    close(epollfd);
    delete[] users;
    delete pool;
    return 0;
}