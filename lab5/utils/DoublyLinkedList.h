#pragma once

template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node *next, *prev;
        Node(T val) : data(val), next(nullptr), prev(nullptr) {}
    };
    Node *head, *tail;
    int   count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }

    void push_back(T val) {
        Node* n = new Node(val);
        if (!head) { head = tail = n; }
        else { tail->next = n; n->prev = tail; tail = n; }
        count++;
    }

    void remove(T val) {
        Node* curr = head;
        while (curr) {
            if (curr->data == val) {
                if (curr->prev) curr->prev->next = curr->next; else head = curr->next;
                if (curr->next) curr->next->prev = curr->prev; else tail = curr->prev;
                delete curr; count--; return;
            }
            curr = curr->next;
        }
    }

    void clear() {
        Node* curr = head;
        while (curr) { Node* nx = curr->next; delete curr; curr = nx; }
        head = tail = nullptr; count = 0;
    }

    int size() const { return count; }

    struct Iterator {
        Node* cur;
        bool operator!=(const Iterator& o) const { return cur != o.cur; }
        void operator++() { if (cur) cur = cur->next; }
        T    operator*()  { return cur->data; }
    };
    Iterator begin() { return {head}; }
    Iterator end()   { return {nullptr}; }
};
