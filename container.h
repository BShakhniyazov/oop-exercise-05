#pragma once

#include <memory>

//list
template<class T>
class List
{
private:
    //list element
    struct Element
    {
        T data;
        std::shared_ptr<Element> next;
        std::weak_ptr<Element> prev;

        Element() {}
        Element(T d) : data(d) {}
        Element(T d, std::shared_ptr<Element> n) : data(d), next(n) {}
        Element(T d, std::shared_ptr<Element> n, std::shared_ptr<Element> p) : data(d), next(n), prev(p) {}
        Element(T d, std::shared_ptr<Element> n, std::weak_ptr<Element> p) : data(d), next(n), prev(p) {}
    };
    //head of list
    std::shared_ptr<Element> head, tail;

public:
    //iterator
    class iterator
    {
    private:
        friend class List; //to get access to List::Element
        std::weak_ptr<List::Element> ukazatel;

    public:
        using difference_type = int;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        //operators
        reference operator*()
        {
            if (!ukazatel.lock()->next) throw std::out_of_range("trying to get value of unexisting element");
            return ukazatel.lock()->data;
        }
        pointer operator->()
        {
            if (!ukazatel.lock()->next) throw std::out_of_range("trying to access unexisting element");
            return &(ukazatel.lock()->data);
        }
        iterator& operator++()
        {
            if (!ukazatel.lock()->next) throw std::out_of_range("moved further than end()");
            ukazatel = (*ukazatel.lock()).next;
            return *this;
        }
        bool operator!=(const iterator& other)
        {
            return ukazatel.lock() != other.ukazatel.lock();
        }
    };
    friend class iterator;

    //list itself
    List()
    {
        head = tail = std::make_shared<Element>(Element());
    }
    ~List() {}

    //get iterators
    iterator begin()
    {
        iterator i;
        i.ukazatel = head;
        return i;
    }
    iterator end()
    {
        iterator i;
        i.ukazatel = tail;
        return i;
    }
    //main functions
    //check for empty
    bool empty() const { return !head; }
    //get size
    size_t size() const
    {
        size_t sz = 0;
        std::weak_ptr<Element> w = head;
        while (w.lock() != tail) { w = w.lock()->next; sz++; }
        return sz;
    }
    //clear list
    void clear()
    {
        head = tail;
    }
    //insert element in list
    void insert(iterator i, const T &val)
    {
        if (!(i.ukazatel.lock()->prev.lock())) //first element
        {
            head = std::make_shared<Element>(Element(val, head));
            head->next->prev = head;
        }
        else //at least 1 element before
        {
            auto el = std::make_shared<Element>(Element(val, i.ukazatel.lock(), i.ukazatel.lock()->prev));
            el->prev.lock()->next = el;
            el->next->prev = el;
        }
    }
    //remove elements from list
    iterator erase(iterator i)
    {
        if (i.ukazatel.lock() == tail) throw std::out_of_range("you can't remove end()");
        auto ret = i;
        ++ret;

        if (!(i.ukazatel.lock()->prev.lock())) //first element
        {
            head->next->prev = std::weak_ptr<Element>();
            head = head->next;
        }
        else //at least 1 element before
        {
            auto el = i.ukazatel.lock();
            el->next->prev = el->prev;
            el->prev.lock()->next = el->next;
        }
        return ret;
    }

    //by index
    T operator[](int i) { return *std::next(begin(), i); }
};
