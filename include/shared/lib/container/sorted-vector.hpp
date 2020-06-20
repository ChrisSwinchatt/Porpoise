#pragma once

#include <algorithm.hpp>
#include <comparer.hpp>
#include <memory.hpp>
#include <string.hpp>

#include <porpoise/assert.hpp>

/// A resizeable ordered container which allows duplicate values.
template <class T, class Allocator=abort_allocator<T>, class Comparer=default_comparer<T>>
struct sorted_vector {
    using allocator_type  = Allocator;
    using size_type       = typename allocator_type::size_type;
    using comparer_type   = Comparer;
    using value_type      = typename allocator_type::value_type;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using iterator        = T*;
    using const_iterator  = const T*;

    static constexpr size_type DEFAULT_CAPACITY = 1;

    explicit sorted_vector(const allocator_type& allocator = allocator_type(), const comparer_type& comparer = comparer_type())
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(0)
    , _comparer(comparer)
    , _deallocate(true)
    , _size(0)
    {
        PORPOISE_ASSERT(allocator != nullptr);
        _buffer   = allocator.allocate(DEFAULT_CAPACITY);
        _capacity = DEFAULT_CAPACITY;
    }

    sorted_vector(pointer buffer, size_type size, size_type capacity, const allocator_type& allocator = allocator_type(), const comparer_type& comparer = comparer_type())
    : _allocator(allocator)
    , _buffer(buffer)
    , _capacity(capacity)
    , _comparer(comparer)
    , _deallocate(false)
    , _size(size)
    {}

    explicit sorted_vector(size_type capacity, const allocator_type& allocator = allocator_type(), const comparer_type& comparer = comparer_type())
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(capacity)
    , _comparer(comparer)
    , _deallocate(true)
    , _size(0)
    {
        _buffer = allocator.allocate(capacity);
    }

    sorted_vector(size_type size, const_reference value, const allocator_type& allocator = allocator_type(), const comparer_type& comparer = comparer_type())
    : _allocator(allocator)
    , _buffer(nullptr)
    , _capacity(size)
    , _comparer(comparer)
    , _deallocate(true)
    , _size(size)
    {
        _buffer = allocator.allocate(size);
        for (auto it = begin(); it < end(); it++)
        {
            *it = value;
        }

        PORPOISE_CHECK_EQUAL(size, _buffer.size());
    }

    explicit sorted_vector(sorted_vector<T>&& other)
    : _allocator(other._allocator)
    , _buffer(other._buffer)
    , _capacity(other._capacity)
    , _comparer(other._comparer)
    , _deallocate(other._deallocate)
    , _size(other._size)
    {
        other._buffer   = nullptr;
        other._capacity = 0;
        other._size     = 0;
    }

    void operator=(sorted_vector<T>&& other)
    {
        _allocator      = other._allocator;
        _buffer         = other._buffer;
        _capacity       = other._capacity;
        _comparer       = other._comparer;
        _deallocate     = other._deallocate;
        _size           = other._size;
        other._buffer   = nullptr;
        other._capacity = 0;
        other._size     = 0;
    }

    virtual ~sorted_vector()
    {
        if (_deallocate)
        {
            _allocator.deallocate(_buffer);
        }
    }

    void add(const_reference value)
    {
        if (_size + 1 >= _capacity)
        {
            resize(_size + 1);
        }

        auto index = get_insert_index(value);
        if (index < _size)
        {
            memmove(_buffer + index, _buffer + _size, _size - index);
        }

        _buffer[index] = value;
        ++_size;
    }

    bool remove(const_reference value)
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

    pointer find(const_reference value)
    {
        return const_cast<pointer>(binary_search(value, begin(), end(), _comparer));
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

    reference at(size_type index)
    {
        if (index >= _size)
        {
            PORPOISE_ABORT("Index " << index << " out of range for container of size " << _size);
        }

        return _buffer[index];
    }

    const_reference at(size_type index) const
    {
        if (index >= _size)
        {
            PORPOISE_ABORT("Index " << index << " out of range for container of size " << _size);
        }

        return _buffer[index];
    }

    reference operator[](size_type index)
    {
        PORPOISE_ASSERT(index < _size);
        return _buffer[index];
    }

    const_reference operator[](size_type index) const
    {
        PORPOISE_ASSERT(index < _size);
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
            return;
        }

        if (next == _size)
        {
            return;
        }

        if (next < 2*_size)
        {
            next = 2*_size;
        }

        internal_reallocate(next);
    }

    void trim_excess()
    {
        if (_capacity > _size)
        {
            internal_reallocate(_size);
        }
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
    allocator_type _allocator;
    pointer _buffer;
    size_type _capacity;
    comparer_type _comparer;
    bool _deallocate;
    size_type _size;

    size_type get_insert_index(const_reference value)
    {
        for (auto i = 0UL; i < _size; i++)
        {
            if (_comparer(value, _buffer[i]))
            {
                return i;
            }
        }

        return _size;
    }

    void internal_reallocate(size_type next)
    {
        auto p = _allocator.reallocate(_buffer, next);
        if (p == nullptr)
        {
            PORPOISE_ABORT("Out of memory");
        }

        if (p != _buffer)
        {
            copy_forwards(_buffer, _buffer + _size, p);
            _allocator.deallocate(_buffer);
            _buffer = p;
        }

        _capacity = next;
    }
};
