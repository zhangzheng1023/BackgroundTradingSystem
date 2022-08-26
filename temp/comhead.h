#ifndef COMHEAD_H
#define COMHEAD_H

#include <iostream>
#include <memory>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {};
    ListNode(int val) : val(val), next(nullptr) {};
    ListNode(int val, ListNode *next) : val(val), next(next) {};
};

class InitLinkedList {
public:
    InitLinkedList();
    void ShowAll();
    void reset();
    ListNode *head;
};

// 5 5 4 8 0 12 2 2 9 1 11
InitLinkedList::InitLinkedList() {
    vector<int> initList = {5, 4, 8, 0, 12, 2, 2, 9, 1, 11};
    this->head = new ListNode(initList[0]);
    ListNode *tail = head;
    for (int val:initList) {
        ListNode *temp = new ListNode(val);
        tail->next = temp;
        tail = temp;
    }
}

void InitLinkedList::ShowAll() {
    ListNode *temp = head;
    while (temp) {
        cout << temp->val << " ";
        temp = temp->next;
    }
    cout << endl;
}

void InitLinkedList::reset() {
    InitLinkedList();
}

#endif