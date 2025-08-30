#pragma once
#include <stdexcept>
#include <memory>
#include <functional>
#include "DynamicArray.hpp"
#include "LinkedList.hpp"


template <typename T>
class Sequence {
public:
    virtual Sequence<T>* CreateEmptySequence() const = 0;
    virtual Sequence<T>* AppendInternal(const T& item) = 0;
    virtual Sequence<T>* PrependInternal(const T& item) = 0;
    virtual Sequence<T>* InsertAtInternal(const T& item, int index) = 0;
    virtual Sequence<T>* ConcatInternal(const Sequence<T>* other) = 0;

    virtual ~Sequence() = default;

    virtual const T& GetFirst() const = 0;
    virtual const T& GetLast() const = 0;
    virtual const T& Get(int index) const = 0;

    virtual T& GetFirst() = 0;
    virtual T& GetLast() = 0;
    virtual T& Get(int index) = 0;

    virtual int GetLength() const = 0;

    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* other) = 0;

    virtual T& operator[] (int index) = 0;

    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    Sequence<T>* Map(std::function<T(T)> mapper) const {
        Sequence<T>* result = this->CreateEmptySequence();
        for (int i = 0; i < this->GetLength(); ++i) {
            result = result->Append(mapper(this->Get(i)));
        }
        return result;
    }

    Sequence<T>* Map(std::function<T(T, int)> mapper) const {
        Sequence<T>* result = this->CreateEmptySequence();
        for (int i = 0; i < this->GetLength(); ++i) {
            result->Append(mapper(this->Get(i), i));
        }
        return result;
    }

    Sequence<T>* Where(std::function<bool(T)> wherer) const {
        Sequence<T>* result = this->CreateEmptySequence();
        for (int i = 0; i < this->GetLength(); ++i) {
            if (wherer(this->Get(i))) {
                result = result->Append(this->Get(i));
            }
        }
        return result;
    }

    T& Reduce(std::function<T(T, T)> reducer, const T& startVal) const {
        T accumulator = startVal;
        for (int i = 0; i < this->GetLength(); ++i) {
            accumulator = reducer(accumulator, this->Get(i));
        }
        return accumulator;
    }

    class Iterator {
    private:
        Sequence<T>* sequence;
        int index;
    
    public:
        Iterator(Sequence<T>* seq, int ind) : sequence(seq), index(ind) {}

        T& operator*() {
            return sequence->Get(index);
        }

        Iterator& operator++() {
            ++index;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return index != other.index;
        }
    };

    Iterator begin() {
        return Iterator(this, 0);
    }

    Iterator end() {
        return Iterator(this, this->GetLength());
    }
};


template <typename T> class MutableArraySequence;
template <typename T> class ImmutableArraySequence;

template <typename T>
class ArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* data;

    virtual Sequence<T>* AppendInternal(const T& item) override {
        this->data->Resize(data->GetSize() + 1);
        this->data->Set(item, this->data->GetSize() - 1);
        return this;
    }

    virtual Sequence<T>* PrependInternal(const T& item) override {
        this->data->Resize(data->GetSize() + 1);

        for (int i = this->data->GetSize() - 1; i > 0; --i) {
            this->data->Set(this->data->Get(i - 1), i);
        }

        this->data->Set(item, 0);
        return this;
    }

    virtual Sequence<T>* InsertAtInternal(const T& item, int index) override {
        this->data->Resize(data->GetSize() + 1);
        if (index == 0)
            return AppendInternal(item);

        for (int i = this->data->GetSize() - 1; i >= index; --i) {
            this->data->Set(this->data->Get(i - 1), i);
        }

        this->data->Set(item, index);
        return this;
    }

    virtual Sequence<T>* ConcatInternal(const Sequence<T>* other) override {
        for (int i = 0; i < other->GetLength(); ++i) {
            this->Append(other->Get(i));
        }
        return this;
    }

protected:
    virtual Sequence<T>* Instance() = 0;
    virtual ArraySequence<T>* CreateEmptyArraySequence() const = 0;

public:
    ArraySequence() : data(new DynamicArray<T>()) {}
    ArraySequence(int sz) : data(new DynamicArray<T>(sz)) {}
    ArraySequence(const T* items, int count) : data(new DynamicArray<T>(items, count)) {}
    ArraySequence(const Sequence<T>& other) : data(new DynamicArray<T>(other.GetSize())) {
        for (int i = 0; i < other.GetSize(); ++i) {
            data->Append(other.Get(i));
        }
    }
    ArraySequence(ArraySequence<T>&& other) noexcept : data(other.data) {
        other.data = nullptr;
    }
    ArraySequence(const ArraySequence<T>& other) : data(new DynamicArray<T>(*other.data)) {}

    ~ArraySequence() override {
        delete data;
    }

    virtual int GetLength() const override {
        return this->data->GetSize();
    }

    int GetCapacity() const {
        return this->data->GetCapacity();
    }

    void Resize(int newSize) {
        this->data->Resize(newSize);
    }

    const T& GetFirst() const override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get first element");
        }

        return this->data->Get(0);
    }

    const T& GetLast() const override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get last element");
        }

        return data->Get(data->GetSize() - 1);
    }

    const T& Get(int index) const override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }

        return data->Get(index);
    }

    T& GetFirst() override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get first element");
        }

        return this->data->Get(0);
    }

    T& GetLast() override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get last element");
        }

        return data->Get(data->GetSize() - 1);
    }

    T& Get(int index) override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }

        return data->Get(index);
    }

    T& operator[] (int index) override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }
        return (*data)[index];
    }

    ArraySequence<T>& operator=(const ArraySequence<T>& other) {
        if (this!= &other) {
            delete data;
            data = new DynamicArray<T>(*other.data);
        }
        return *this;
    }

    ArraySequence<T>& operator=(ArraySequence<T>&& other) noexcept {
        if (this!= &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    ArraySequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (std::min(startIndex, endIndex) < 0 || std::max(startIndex, endIndex) >= data->GetSize()) {
            throw std::out_of_range("ArraySequence index out of range");
        }

        ArraySequence<T>* ret = this->CreateEmptyArraySequence();

        if (startIndex <= endIndex) {
            for (int i = startIndex; i <= endIndex; ++i) {
                ret->Append(data->Get(i));
            }
        } else {
            for (int i = startIndex; i >= endIndex; --i) {
                ret->Append(data->Get(i));
            }
        }

        return ret;
    }

    virtual Sequence<T>* Append(const T& item) override {
        return Instance()->AppendInternal(item);
    }

    virtual Sequence<T>* Prepend(const T& item) override {
        return Instance()->PrependInternal(item);
    }

    virtual Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("ArraySequence index out of range");
        }

        return Instance()->InsertAtInternal(item, index);
    }

    virtual Sequence<T>* Concat(const Sequence<T>* other) override {
        return this->Instance()->ConcatInternal(other);
    }
};

template <typename T> class MutableListSequence;
template <typename T> class ImmutableListSequence;

template <typename T>
class ListSequence : public Sequence<T> {
private:
    LinkedList<T>* data;

    virtual Sequence<T>* AppendInternal(const T& item) override {
        this->data->Append(item);
        return this;
    }

    virtual Sequence<T>* PrependInternal(const T& item) override {
        this->data->Prepend(item);
        return this;
    }

    virtual Sequence<T>* InsertAtInternal(const T& item, int index) override {
        this->data->InsertAt(item, index);
        return this;
    }

    virtual Sequence<T>* ConcatInternal(const Sequence<T>* other) override {
        for (int i = 0; i < other->GetLength(); ++i) {
            this->Append(other->Get(i));
        }
        return this;
    }

protected:
    virtual Sequence<T>* Instance() = 0;
    virtual ListSequence<T>* CreateEmptyListSequence() const = 0;

public:
    ListSequence() : data(new LinkedList<T>()) {}
    ListSequence(const T* items, int count) : data(new LinkedList<T>(items, count)) {}
    ListSequence(const Sequence<T>& other) : data(new LinkedList<T>()) {
        for (int i = 0; i < other.GetSize(); ++i) {
            data->Set(i, other.Get(i));
        }
    }
    ListSequence(ListSequence<T>&& other) noexcept : data(other.data) {
        other.data = nullptr;
    }
    ListSequence(const ListSequence<T>& other) : data(new LinkedList<T>(*other.data)) {}

    ~ListSequence() override {
        delete this->data;
    }

    virtual int GetLength() const override {
        return this->data->GetSize();
    }

    const T& GetFirst() const override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get first element");
        }

        return this->data->GetFirst();
    }

    const T& GetLast() const override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get last element");
        }

        return this->data->GetLast();
    }

    const T& Get(int index) const override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }

        return this->data->Get(index);
    }

    T& GetFirst() override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get first element");
        }

        return this->data->GetFirst();
    }

    T& GetLast() override {
        if (this->GetLength() == 0) {
            throw std::out_of_range("Sequence is empty - cannot get last element");
        }

        return this->data->GetLast();
    }

    T& Get(int index) override {
        if (index < 0 || index >= data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }

        return this->data->Get(index);
    }

    T& operator[] (int index) override {
        return this->data->Get(index);
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this!= &other) {
            delete this->data;
            this->data = new LinkedList<T>(*other.data);
        }
        return *this;
    }

    LinkedList<T>& operator=(LinkedList<T>&& other) noexcept {
        if (this!= &other) {
            delete this->data;
            this->data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    ListSequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (std::min(startIndex, endIndex) < 0 || std::max(startIndex, endIndex) >= data->GetSize()) {
            throw std::out_of_range("ArraySequence index out of range");
        }

        ListSequence<T>* ret = this->CreateEmptyListSequence();
        ret->data = this->data->GetSubList(startIndex, endIndex);

        return ret;
    }

    virtual Sequence<T>* Append(const T& item) override {
        return this->Instance()->AppendInternal(item);
    }

    virtual Sequence<T>* Prepend(const T& item) override {
        return this->Instance()->PrependInternal(item);
    }

    virtual Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0 || index >= this->data->GetSize()) {
            throw std::out_of_range("Sequence index out of range");
        }
        return this->Instance()->InsertAtInternal(item, index);
    }

    virtual Sequence<T>* Concat(const Sequence<T>* other) override {
        return this->Instance()->ConcatInternal(other);
    }
};



struct MutableSequenceTag {};
struct ImmutableSequenceTag {};


template <typename T>
class MutableArraySequence : public ArraySequence<T> {
public:
    using tag = MutableSequenceTag;

    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(int sz) : ArraySequence<T>(sz) {}
    MutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}
    MutableArraySequence(const ArraySequence<T>& other) : ArraySequence<T>(other) {}
    MutableArraySequence(ArraySequence<T>&& other) : ArraySequence<T>(std::move(other)) {}

    virtual Sequence<T>* CreateEmptySequence() const override { 
        return new MutableArraySequence<T>();
    }
    virtual ArraySequence<T>* CreateEmptyArraySequence() const override {
        return new MutableArraySequence<T>();
    }
    virtual ArraySequence<T>* Instance() override {
        return this;
    }
};


template <typename T>
class ImmutableArraySequence : public ArraySequence<T> {
private:
    ArraySequence<T>* Clone() const {
        return new ImmutableArraySequence<T>(*this);
    }

public:
    using tag = ImmutableSequenceTag;

    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(int sz) : ArraySequence<T>(sz) {}
    ImmutableArraySequence(const T* items, int count) : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const Sequence<T>& other) : ArraySequence<T>(other) {}
    ImmutableArraySequence(ArraySequence<T>&& other) : ArraySequence<T>(std::move(other)) {}

    virtual Sequence<T>* CreateEmptySequence() const override { 
        return new ImmutableArraySequence<T>();
    }
    virtual ArraySequence<T>* CreateEmptyArraySequence() const override {
        return new ImmutableArraySequence<T>();
    }
    virtual ArraySequence<T>* Instance() override {
        return Clone();
    }
};


template <typename T>
class MutableListSequence : public ListSequence<T> {
public:
    using tag = MutableSequenceTag;

    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}
    MutableListSequence(const Sequence<T>& other) : ListSequence<T>(other) {}
    MutableListSequence(ListSequence<T>&& other) : ListSequence<T>(std::move(other)) {}

    virtual Sequence<T>* CreateEmptySequence() const override { 
        return new MutableListSequence<T>();
    }
    virtual ListSequence<T>* CreateEmptyListSequence() const override {
        return new MutableListSequence<T>();
    }
    virtual ListSequence<T>* Instance() override {
        return this;
    }
};


template <typename T>
class ImmutableListSequence : public ListSequence<T> {
private:
    ListSequence<T>* Clone() const {
        return new ImmutableListSequence<T>(*this);
    }

public:
    using tag = ImmutableSequenceTag;

    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(const T* items, int count) : ListSequence<T>(items, count) {}
    ImmutableListSequence(const Sequence<T>& other) : ListSequence<T>(other) {}
    ImmutableListSequence(ListSequence<T>&& other) : ListSequence<T>(std::move(other)) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequence<T>(other) {}

    virtual Sequence<T>* CreateEmptySequence() const override { 
        return new ImmutableListSequence<T>();
    }
    virtual ListSequence<T>* CreateEmptyListSequence() const override {
        return new ImmutableListSequence<T>();
    }
    virtual ListSequence<T>* Instance() override {
        return Clone();
    }
};


template <typename T1, typename T2>
Sequence<std::pair<T1, T2>>* zip(const Sequence<T1>* seq1, const Sequence<T2>* seq2) {
    int min_length = std::min(seq1->GetLength(), seq2->GetLength());
    auto* result = new MutableArraySequence<std::pair<T1, T2>>();

    for (int i = 0; i < min_length; ++i) {
        result->Append(std::make_pair(seq1->Get(i), seq2->Get(i)));
    }

    return result;
}

template <typename T1, typename T2>
std::pair<Sequence<T1>*, Sequence<T2>*> unzip(const Sequence<std::pair<T1, T2>>* zipped) {
    auto* seq1 = new MutableArraySequence<T1>();
    auto* seq2 = new MutableArraySequence<T2>();

    for (auto& pair : *zipped) {
        seq1->Append(pair.first);
        seq2->Append(pair.second);
    }

    return std::make_pair(seq1, seq2);
}