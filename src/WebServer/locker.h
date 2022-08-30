//ifndef主要是防止源文件多次包含相同的头文件，这样若头文件中有类定义等，会出现类重复定义的编译错误
#ifndef LOCKER_H
#define LOCKER_H
#include<pthread.h>
#include<exception>
#include<semaphore.h>
#include<iostream>
//线程同步机制封装类
//1、互斥锁类
class locker{
public:
    locker(){
        //该函数是以动态方式（在堆中创建，需要销毁）创建锁，第二个参数是锁的类型，默认是快速互斥锁。返回值为0时成功创建
        //还可以使用宏初始化（静态初始化：编译时初始化。与之相对的动态初始化，可以是加载时初始化，可以是使用时初始化）
        if(pthread_mutex_init(&m_mutex,NULL)!=0){
            throw std::exception();//exception：异常类
        }
    }
    ~locker(){
        //在Linux中，互斥锁并不占用任何资源，该函数只检查锁的状态
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock(){//上锁
        //上锁后，其他线程请求加锁后会阻塞
        return pthread_mutex_lock(&m_mutex)==0;
    }
    bool unlock(){//解锁
        return pthread_mutex_unlock(&m_mutex)==0;
    }
    pthread_mutex_t *get(){//获得锁
        return &m_mutex;
    }
private:
    pthread_mutex_t m_mutex;//互斥锁
};
//2、条件变量类（通知有已经生产好的产品可以用了
class cond{
public:
    cond(){
        //该函数为动态初始化，也可以使用宏静态初始化
        if(pthread_cond_init(&m_cond,NULL)!=0){
            throw std::exception();
        }
    }
    ~cond(){
        //仍然不用释放内存空间，只是可以再次初始化（Linux的设计是这样的，但我不知道为什么
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t *m_mutex){
        //等待消息被生产出来，线程阻塞，即等待条件变量为真，在等待的时候未上锁，若为真了则上锁，并返回0
        return pthread_cond_wait(&m_cond,m_mutex)==0;
    }
    bool timedwait(pthread_mutex_t *m_mutex,struct timespec t){
        //线程阻塞时间t
        return pthread_cond_timedwait(&m_cond,m_mutex,&t)==0;
    }
    bool signal(){
        //唤醒等待的1个或多个线程。唤醒后解除Wait阻塞，试图获取锁
        return pthread_cond_signal(&m_cond)==0;
    }
    bool broadcast(){
        //唤醒所有线程
        return pthread_cond_broadcast(&m_cond)==0;
    }
private:
    pthread_cond_t m_cond;
};
//3、信号量类（通知具体有多少个产品可用
class sem{
public:
    sem(){
        //第二个参数是是否在进程间共享，为0则只在当前进程的所有线程中共享，为1则在进程中共享
        //第三个参数是信号量的值
        if(sem_init(&m_sem,0,0)!=0){
            throw std::exception();
        }
    }
    sem(int num){
        if(sem_init(&m_sem,0,num)!=0){
            throw std::exception();
        }
    }
    ~sem(){
        sem_destroy(&m_sem);
    }
    //等待信号量：为0时等待，大于0时占用一个产品并解除阻塞
    bool wait(){
        return sem_wait(&m_sem)==0;
    }
    //增加信号量：释放占用的产品
    bool post(){
        return sem_post(&m_sem)==0;
    }
private:
    //本质上是一个长整型的数
    sem_t m_sem;
};
/*
条件变量和信号量的区别
1、条件变量有唤醒线程的方法，信号量无
2、信号量可以表征公共资源的数量，条件变量不可
3、信号量可以用于进程间通信，也可以用于同一进程内的线程间通信，但条件变量仅用于线程间通信
*/
#endif