#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<pthread.h>
#include<list>
#include<exception>
#include<iostream>
#include"locker.h"
//线程池类，将它定义为模板类是为了代码复用，模板参数T是任务类
template<typename T>
class threadpool{
public:
    threadpool(int thread_num=8,int max_request=10000);
    ~threadpool();
    bool append(T* request);
private:
    //线程数量
    int m_thread_number;
    //线程数组，数量为m_thread_number
    pthread_t * m_threads;
    //请求队列中最大任务数量
    int m_max_requests;
    //请求队列
    std::list<T*> m_workqueue;
    //用于请求队列的互斥锁
    locker m_queuelocker;
    //用于判断是否有任务需要处理的信号量
    sem m_queuestat;
    //是否结束线程
    bool m_stop;
    //工作函数必须是静态的
    static void * worker(void * arg);
    //线程池的启动
    void run();
};

//模板类函数在类外定义时，前面的类域要写明模板参数
template<typename T>
threadpool<T>::threadpool(int thread_num,int max_request):
m_thread_number(thread_num),m_max_requests(max_request),m_stop(false),m_threads(nullptr){
    if(thread_num<=0||max_request<=0)throw std::exception();
    m_threads=new pthread_t[m_thread_number];
    if(!m_threads)throw std::exception();
    //创建thread_num个线程，并将他们设置为线程分离（
    for(int i=0;i<thread_num;++i){
        std::cout<<"create the "<<i<<"th thread"<<std::endl;
        //worker必须是一个静态函数
        //m_threads是一个线程数组指针，该数组以链表的形式存在
        //pthread_create第二个参数是设置新建线程的一些属性，比如说调度方式、栈空间大小
        //第三个参数是新建线程需要执行的函数，该函数的形参和返回值必须为void*
        //第四个参数void*arg是指传递给第三个参数（线程执行函数）的参数
        //为了使得worker函数能够访问到线程池中的资源（请求队列等），传递this指针
        if(pthread_create(m_threads+i,NULL,worker,this)!=0){
            //对于数组的内存空间释放，最好用delete[]，尤其是自定义的类数组
            delete[] m_threads;
            throw std::exception();
        }
        //detach可以将子线程和主线程断开联系，其退出状态不会被其他线程获取，而是自我释放资源，不会产生僵尸进程
        //不能对detach状态的线程进行状态回收，即不能再使用pthread_join函数
        if(pthread_detach(m_threads[i])){
            delete[] m_threads;
            throw std::exception();
        }

    }
}

template<typename T>
threadpool<T>::~threadpool(){
    delete[] m_threads;
    m_stop=true;
}

template<typename T>
bool threadpool<T>::append(T* request){
    m_queuelocker.lock();
    if(m_workqueue.size()>=m_max_requests){
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void * threadpool<T>::worker(void * arg){
    threadpool * pool = (threadpool *)arg;
    //线程池需要工作起来，即去等待任务、处理任务
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run(){
    //若线程不停止就一直循环
    while(!m_stop){
        //一直等待需要处理的任务，即一直阻塞到有任务需要处理
        m_queuestat.wait();
        //察觉到有任务需要处理后，就需要去任务队列里取出任务，因为是公共资源，所以要上锁
        m_queuelocker.lock();
        //再次确认是否真的有任务需要处理
        if(m_workqueue.empty()){
            //没有任务就把之前拿到的锁解锁
            m_queuelocker.unlock();
            continue;
        }
        //获取任务
        T* request=m_workqueue.front();
        m_workqueue.pop_front();
        //获取后解锁
        m_queuelocker.unlock();
        //判断是否成功获取
        if(!request)continue;
        //处理任务
        request->process();
    }
}

#endif