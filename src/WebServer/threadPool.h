#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include "locker.h"
#include <exception>
#include <cstdio>

// 线程池类 为了代码复用 T为任务类
template<typename T>
class ThreadPool {
public:
    ThreadPool(int thread_number = 8, int max_requests = 10000);
    ~ThreadPool();

    bool append(T *request);

private:

    // worker 函数为线程池中线程执行的代码 必须为静态函数
    static void *worker(void *arg);
    // 线程池运行
    void run();

    int m_thread_number;  // 线程数量
    pthread_t *m_threads;  // 线程池数组

    // 请求队列中的最多允许额 等待处理的请求数量
    int m_max_requests;

    // 请求队列
    std::list<T*> m_workqueue;

    // 互斥锁
    Locker m_queuelocker;

    // 信号量 判断是否有任务需要处理
    Sem m_queuestat;

    // 是否结束线程
    bool m_stop;

};

// 初始化列表 
template<typename T>
ThreadPool<T>::ThreadPool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false), m_threads(NULL) {
    if (thread_number <= 0 || max_requests <= 0) {
        throw std::exception();
    }

    // 创建线程池容器的数组
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads) {
        throw std::exception();
    }

    // 创建thread_num个线程，并将其设置为线程脱离
    for (int i = 0; i < m_thread_number; ++i) {
        printf("create the %dth thread\n", i);
        // worker 必须是一个静态的函数 向worker中传入this指针
        if (pthread_create(m_threads + i, NULL, worker, this) != 0) {
            // 出错了需要释放掉数组
            delete[] m_threads;
            throw std::exception();
        }

        if (pthread_detach(m_threads[i])) {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
ThreadPool<T>::~ThreadPool() {
    delete[] m_threads;
    m_stop = true;
}

template<typename T>
bool ThreadPool<T>::append(T *request) {
    m_queuelocker.lock(); // 上锁
    if (m_workqueue.size() > m_max_requests) {
        m_queuelocker.unlock();
        return false;
    }

    // 可以添加任务
    m_workqueue.emplace_back(request);
    m_queuelocker.unlock();
    m_queuestat.post(); // 信号量增加
    return true;
}

template<typename T>
void *ThreadPool<T>::worker(void *arg) {
    ThreadPool *pool = (ThreadPool *) arg;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run() {
    while (!m_stop) {
        m_queuestat.wait();
        if (m_workqueue.empty()) {
            m_queuelocker.unlock();
            continue;
        }

        // 取出任务队列任务
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if (!request) {
            continue;
        }

        request->process();

    }

}

#endif