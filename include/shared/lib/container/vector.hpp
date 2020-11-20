#pragma once

#include <string.hpp>

#include <algorithm.hpp>
#include <comparer.hpp>
#include <memory.hpp>

#include <porpoise/assert.hpp>

/// A resizeable unordered container.
template <class T, class Allocator=basic_allocator<T>, class Comparer=default_comparer<T>>
struct vector {
    using allocator_type  = Allocator;
    using size_type       = typename allocator_type::size_type;
    using comparer_type   = Comparer;
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using iterator        = T*;
    using const_iterator  = const T*;

    static constexpr size_type DEFAULT_CAPACITY = 1;

    explicit vector(allocator_type& allocator = abort_allocator<value_type>::instance)
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(0)
    , _deallocate(true)
    , _size(0)
    {
        PORPOISE_ASSERT(allocator != nullptr);
        _buffer = allocator.allocate(DEFAULT_CAPACITY);
        _capacity = DEFAULT_CAPACITY;
    }

    vector(pointer buffer, size_type size, allocator_type& allocator = abort_allocator<value_type>::instance)
    : _allocator(allocator)
    , _buffer(buffer)
    , _capacity(size)
    , _deallocate(false)
    , _size(size)
    {}

    explicit vector(size_type capacity, allocator_type& allocator = abort_allocator<value_type>::instance)
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(capacity)
    , _deallocate(true)
    , _size(capacity)
    {
        _buffer = allocator.allocate(capacity);
    }

    vector(size_type size, const_reference value, allocator_type& allocator = abort_allocator<value_type>::instance)
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(size)
    , _deallocate(true)
    , _size(size)
    {
        _buffer = allocator.allocate(size);
        for (auto it = begin(); it < end(); it++)
        {
            *it = value;
        }
    }

    vector(const vector<value_type>& other)
    : _allocator(other._allocator)
    , _buffer(nullptr)
    , _capacity(other._capacity)
    , _deallocate(true)
    , _size(other._size)
    {
        _buffer = _allocator.allocate(_size);
        copy_forwards(other.begin(), other.end(), _buffer);
    }

    void operator=(const vector<value_type>& other)
    {
        _allocator = other._allocator;
        _capacity = other._capacity;
        _deallocate = true;
        _size = other._size;
        _buffer = _allocator.allocate(_size);
        copy_forwards(other.begin(), other.end(), _buffer);
    }

    virtual ~vector()
    {
        if (_deallocate)
        {
            _allocator.deallocate(_buffer);
        }
    }

    void push_back(T value)
    {
        if (_size + 1 >= capacity)
        {
            resize(_size + 1);
        }

        auto index = get_insert_index(value);
        if (index < _size)
        {
            safe_copy(at(index + 1), end(), at(index));
        }

        _buffer[index] = value;
        ++_size;
    }

    bool pop_back(const_reference value)
    {
        if (!_size)
        {
            return false;
        }

        auto p = find(value);
        if (p == nullptr)
        {
            return false;
        }

        safe_copy(p + 1, end(), p);
        return true;
    }

    pointer find(const_reference value, const comparer_type& comparer = comparer_type())
    {
        return const_cast<pointer>(linear_search(value, begin(), end(), comparer));
    }

    pointer begin()
    {
        return _buffer;
    }

    const_pointer begin() const
    {
        return _buffer;
    }

    pointer end()
    {
        return _buffer + _size;
    }

    const_pointer end() const
    {
        return _buffer + _size;
    }

    reference front()
    {
        return _buffer[0];
    }

    const_reference front() const
    {
        return _buffer[0];
    }

    reference back()
    {
        return _buffer[_size - 1];
    }

    const_reference back() const
    {
        return _buffer[_size - 1];
    }

    iterator at(size_type index)
    {
        return _buffer + index;
    }

    const_iterator at(size_type index) const
    {
        return _buffer + index;
    }

    reference operator[](size_type index)
    {
        PORPOISE_ASSERT_LESS(index, _size);
        return _buffer[index];
    }

    const_reference operator[](size_type index) const
    {
        PORPOISE_ASSERT_LESS(index, _size);
        return _buffer[index];
    }

    size_type size() const
    {
        return _size;
    }

    void resize(size_type next)
    {
        if (next < _size)
        {
            _size = next;
        }
    }

    void trim_excess()
    {
        if (_capacity > _size)
        {
            internal_reallocate(_size);
        }
    }

    void reserve(size_type next)
    {
        if (next < _capacity)
        {
            return;
        }

        auto curr = _capacity;
        while (curr <= next)
        {
            curr += _capacity;
        }

        internal_reallocate(curr);
    }

    size_type capacity() const
    {
        return _capacity;
    }

    bool empty() const
    {
        return _size == 0;
    }
private:
    allocator_type& _allocator;
    pointer         _buffer;
    size_type       _capacity;
    bool            _deallocate;
    size_type       _size;

    void internal_reallocate(size_type next)
    {
        auto p = _allocator.reallocate(_buffer, next);
        if (p != _buffer)
        {
            copy_forwards(_buffer, _buffer + _size, p);
            _allocator.deallocate(_buffer);
            _buffer = p;
        }

        _capacity = next;
        _deallocate = true;
    }
};
