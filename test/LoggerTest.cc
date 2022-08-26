#include "logger.h"
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <vector>

using namespace std;

void *thread_job(void *args) {
    cout << "pid = "<< getpid() << " tid = " << gettid() << endl;
    return NULL;
}

int main() {
    // Logger logger;
    // string test_log = "test_log";
    // logger.writeLog(test_log);

    // 启动20个线程

    // pthread_t tids[20];
    // for (int i = 0; i < 20; ++i) {
    //     pthread_create(&tids[i], NULL, thread_job, NULL);
    // }

    // for (int i = 0; i < 20; ++i) {
    //     pthread_join(tids[i], NULL);
    // }

    // vector<string> stream;
    vector<string> stream(10);
    cout << stream[9] << endl;
    string temp = "12343243";
    stream[9] = temp;
    if (stream[9].empty()) cout << 11 << endl;
    else cout << 22 << endl;
    cout << stream[9] << endl;
    return 0;
}