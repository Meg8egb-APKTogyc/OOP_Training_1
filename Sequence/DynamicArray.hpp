#pragma once
#include <iostream>
#include <stdexcept>
#include <algorithm>


template <typename T>
class DynamicArray {
private:
    T* data;
    int size;
    int capacity;

    int _getCapacity(int val) {
        if (val == 0) return 0;

        int ret = 1;
        while (ret < val) ret <<= 1;

        return ret;
    }

    void _checkException(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
    }

public:
    DynamicArray(): data(nullptr), size(0), capacity(0) {}

    DynamicArray(int initialCapacity) : size(initialCapacity), capacity(_getCapacity(initialCapacity)) {
        data = new T[capacity];
    }

    DynamicArray(const T* items, int count) : size(count), capacity(_getCapacity(count)) {
        data = new T[capacity];
        std::copy(items, items + count, data);
    }

    DynamicArray(const DynamicArray& other) : size(other.size), capacity(other.capacity) {
        data = new T[capacity];
        std::copy(other.data, other.data + size, data);
    }

    ~DynamicArray() {
        delete[] data;
    }

    T& operator[](int index) {
        _checkException(index);
        
        return data[index];
    }

    int GetSize() const {
        return size;
    }

    int GetCapacity() const {
        return capacity;
    }

    void Resize(int newSize) {
        int newCapacity = _getCapacity(newSize);
        if (capacity == newCapacity) { 
            size = newSize;
            return;
        }

        T* newData = new T[newCapacity];
        std::copy(data, data + std::min(size, newSize), newData);
        delete[] data;
        data = newData;
        capacity = newCapacity;
        size = newSize;
    }

    void Set(const T& value, int index) {
        _checkException(index);

        data[index] = value;
    }

    T& Get(int index) const {
        _checkException(index);

        return data[index];
    }
};