#include "locker.h"
#include <exception> // 抛出异常

// Locker类的实现
Locker::Locker() {
    if (pthread_mutex_init(&m_mutex, NULL) != 0) {
        throw std::exception();
    }
}

Locker::~Locker() {
    pthread_mutex_destroy(&m_mutex);
}

bool Locker::lock() {
    return pthread_mutex_lock(&m_mutex) == 0;
}

bool Locker::unlock() {
    return pthread_mutex_unlock(&m_mutex) == 0;
}

pthread_mutex_t *Locker::get() {
    return &m_mutex;
}

// Cond类的实现
Condition::Condition() {
    if (pthread_cond_init(&m_cond, NULL) != 0) {
        throw std::exception();
    }
}

Condition::~Condition() {
    if (pthread_cond_destroy(&m_cond) != 0) {
        throw std::exception();
    }
}

bool Condition::wait(pthread_mutex_t *mutex) {
    return pthread_cond_wait(&m_cond, mutex) == 0;
}

bool Condition::timedwait(pthread_mutex_t *mutex, timespec t) {
    return pthread_cond_timedwait(&m_cond, mutex, &t) == 0;
}

bool Condition::signal() {
    return pthread_cond_signal(&m_cond) == 0;
}

bool Condition::broadcast() {
    return pthread_cond_broadcast(&m_cond) == 0;
}

// 信号量类的实现
Sem::Sem() {
    if (sem_init(&m_sem, 0, 0) != 0) {
        throw std::exception();
    }
}

Sem::Sem(int num) {
    if (sem_init(&m_sem, 0, num) != 0) {
        throw std::exception();
    }
}

Sem::~Sem() {
    sem_destroy(&m_sem);
}

bool Sem::wait() {
    return sem_wait(&m_sem) == 0;
}

bool Sem::post() {
    return sem_post(&m_sem) == 0;
}