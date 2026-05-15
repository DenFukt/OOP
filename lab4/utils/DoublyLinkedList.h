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
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }

    void push_back(T val) {
        Node* newNode = new Node(val);
        if(!head){
            head = tail = newNode;
        }
        else{
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    void remove(T val){
        Node* curr = head;
        while(curr){
            if(curr->data == val){
                if (curr->prev) curr->prev->next = curr->next;
                if (curr->next) curr->next->prev = curr->prev;
                if (curr == head) head = curr->next;
                if (curr == tail) tail = curr->prev;
                delete curr;
                count--;
                return;
            }
            curr = curr->next;
        }
    }

    void clear(){
        Node* curr = head;
        while(curr){
            Node* next = curr->next;
            delete curr;
            curr = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    int size() const { return count; }

    struct Iterator {
        Node* current;
        bool operator!=(const Iterator& other) { return current != other.current; }
        void operator++() { if (current) current = current->next; }
        T operator*() { return current->data; }
    };
    Iterator begin() { return { head }; }
    Iterator end()   { return { nullptr }; }
};
