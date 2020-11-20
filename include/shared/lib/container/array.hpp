#pragma once

#include <algorithm.hpp>

/// Wrapper around a C array.
template <class T, size_t N>
struct array
{
    using size_type       = size_t;
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using iterator        = T*;
    using const_iterator  = const T*;

    array()
    {}

    /// Initialize with value.
    explicit array(const T& value)
    {
        fill_region(_data, _data + N, value);
    }

    T* begin()
    {
        return _data;
    }

    const T* begin() const
    {
        return _data;
    }

    T* end()
    {
        return _data + N;
    }

    const T* end() const
    {
        return _data + N;
    }

    T& operator[](size_t index)
    {
        return _data[index];
    }

    const T& operator[](size_t index) const
    {
        return _data[index];
    }

    size_t size() const
    {
        return N;
    }
private:
    T _data[N];
};
