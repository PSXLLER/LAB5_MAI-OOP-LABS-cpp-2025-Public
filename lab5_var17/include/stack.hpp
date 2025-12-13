#pragma once

#include <memory_resource>
#include <iterator>
#include <cstddef>
#include <utility>

template<typename T>
class Stack 
{
private:
    struct Node 
    {
        T value;
        Node* next;
    };

    using allocator_type = std::pmr::polymorphic_allocator<Node>;

    allocator_type alloc_;
    Node* head_;

public:
    class Iterator 
    {
    private:
        Node* ptr_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using reference         = T&;
        using pointer           = T*;
        using difference_type   = std::ptrdiff_t;

        explicit Iterator(Node* p) : ptr_(p) {}
        explicit Iterator(const Node* p) : ptr_(const_cast<Node*>(p)) {}
        explicit Iterator(std::nullptr_t) : ptr_(nullptr) {}

        reference operator*() const { return ptr_->value; }
        pointer   operator->() const { return &(ptr_->value); }

        Iterator& operator++() 
        {
            if (ptr_) ptr_ = ptr_->next;
            return *this;
        }

        Iterator operator++(int) 
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
    };

public:
    explicit Stack(std::pmr::memory_resource* rsrc)
        : alloc_(rsrc), head_(nullptr) {}

    ~Stack() 
    {
        clear();
    }

    bool empty() const 
    {
        return head_ == nullptr;
    }

    void push(const T& value) 
    {
        Node* n = alloc_.allocate(1);
        alloc_.construct(n, Node{value, head_});
        head_ = n;
    }

    void push(T&& value) 
    {
        Node* n = alloc_.allocate(1);
        alloc_.construct(n, Node{std::move(value), head_});
        head_ = n;
    }

    void pop() 
    {
        if (!head_) return;
        Node* old = head_;
        head_ = head_->next;

        std::allocator_traits<allocator_type>::destroy(alloc_, old);
        alloc_.deallocate(old, 1);
    }

    void clear() 
    {
        while (!empty()) pop();
    }

    T& top() 
    {
        return head_->value;
    }

    const T& top() const 
    {
        return head_->value;
    }

    Iterator begin()             { return Iterator(head_); }
    Iterator end()               { return Iterator(nullptr); }

    Iterator begin() const       { return Iterator(head_); }
    Iterator end() const         { return Iterator(nullptr); }
};
