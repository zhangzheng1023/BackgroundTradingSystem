/*
@Author zhang_z
@Email zhang_z@bupt.edu.cn
*/

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    int A[3] = {1, 2, 3};
    for (int &val : A) {
        cout << &val << " ";
    }
    cout << endl << A;
}

//int Socket (int family, int type, int protocol) {
//    int n;
//    if ((n = socket(family, type, protocol)) < 0)
//        err_sys("socket error");
//    return n;
//}