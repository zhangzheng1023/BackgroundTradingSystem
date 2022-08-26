#include "comhead.h"

void showList(ListNode *head) {
    while(head) {
        cout << head->val << " ";
    }
    cout << endl;
}

// reverse LinkedList
ListNode *reverseLinkedList(ListNode *head) {
    ListNode *prev = nullptr;
    ListNode *curr = head;
    while (curr) {
        ListNode *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    while (prev)
    {
        cout << prev->val << " ";
        prev = prev->next;
    }
    

    return prev;
}



int main() {
    InitLinkedList list;
    cout << "before reverse" << endl;
    list.ShowAll();
    cout << "after reverse" << endl;
    ListNode *temp = reverseLinkedList(list.head);

    return 0;
}