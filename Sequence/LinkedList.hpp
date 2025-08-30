#pragma once
#include <iostream>
#include <stdexcept>
#include <algorithm>


template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;

        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

    void _checkException(int index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
    }

public:
    LinkedList(): head(nullptr), tail(nullptr), size(0) {}
    LinkedList(const T* items, int count) : head(nullptr), tail(nullptr), size(0) {
        for (int i = 0; i < count; ++i) {
            Append(items[i]);
        }
    }
    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), size(0) {
        for (Node* current = other.head; current != nullptr; current = current->next) {
            Append(current->data);
        }
    }

    ~LinkedList() {
        Clear();
    }

    void Clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }

        tail = nullptr;
        size = 0;
    }

    int GetSize() const { 
        return size;
    }

    void Append(const T& value) {
        Node* newNode = new Node(value);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        ++size;
    }

    void Prepend(const T& value) {
        Node* newNode = new Node(value);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        ++size;
    }

    T& Get(int index) const {
        _checkException(index);
        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    T& GetFirst() const {
        if (size == 0)
            throw std::out_of_range("List is empty");
        return head->data;
    }

    T& GetLast() const {
        if (size == 0)
            throw std::out_of_range("List is empty");
        return tail->data;
    }

    void InsertAt(const T& item, int index) {
        _checkException(index);
        if (index == 0) {
            Prepend(item);
        } else if (index == size) {
            Append(item);
        } else {
            Node* newNode = new Node(item);
            Node* current = head;
            for (int i = 0; i < index; ++i) {
                current = current->next;
            }
            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;
            ++size;
        }
    }

    LinkedList<T>* Concat(const LinkedList<T>* list) {
        LinkedList<T>* result = new LinkedList<T>(*this);

        Node* current = list->head;
        while (current != nullptr) {
            result->Append(current->data);
            current = current->next;
        }

        return result;
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) {
        _checkException(startIndex);
        _checkException(endIndex);

        LinkedList<T>* result = new LinkedList<T>();

        if (startIndex <= endIndex) {
            Node* current = head;
            for (int i = 0; i < startIndex; ++i) {
                current = current->next;
            }
            for (int i = startIndex; i <= endIndex; ++i) {
                result->Append(current->data);
                current = current->next;
            }
        } else {
            Node* current = tail;
            for (int i = size - 1; startIndex < i; --i) {
                current = current->prev;
            }
            for (int i = startIndex; endIndex <= i; --i) {
                result->Append(current->data);
                current = current->prev;
            }
        }

        return result;
    }
};