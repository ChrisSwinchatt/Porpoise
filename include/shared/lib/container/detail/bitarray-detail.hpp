#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

namespace bitarray_detail {
    constexpr size_t QWORD_BITS = sizeof(uint64_t)*CHAR_BIT;

    /// Mutable reference to a bit.
    struct reference {
        explicit reference(uint64_t& value, uint64_t mask)
        : _value(value)
        , _mask(mask)
        {}

        operator bool() const
        {
            return _value & _mask;
        }

        void operator=(bool next)
        {
            // https://graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
            _value = (_value & ~_mask) | (-static_cast<uint64_t>(next) & _mask);
        }
    private:
        uint64_t& _value;
        uint64_t  _mask;
    };

    /// bitarray iterator.
    struct iterator {
        iterator(uint64_t* ptr, size_t pos)
        : _ptr(ptr)
        , _pos(pos)
        {}

        iterator(const iterator&) = default;

        iterator& operator=(const iterator&) = default;

        iterator& operator++()
        {
            _pos++;
            return *this;
        }
        
        iterator operator++(int)
        {
            auto copy = *this;
            _pos++;
            return copy;
        }

        reference operator*()
        {
            return reference(_ptr[_pos/QWORD_BITS], 1 << (_pos%QWORD_BITS));
        }

        bool operator*() const
        {
            return static_cast<bool>(_ptr[_pos/QWORD_BITS] & (1 << (_pos%QWORD_BITS)));
        }

        bool operator<(const iterator& other) const
        {
            return _ptr < other._ptr;
        }

        bool operator<=(const iterator& other) const
        {
            return _ptr <= other._ptr;
        }

        bool operator>(const iterator& other) const
        {
            return _ptr > other._ptr;
        }

        bool operator>=(const iterator& other) const
        {
            return _ptr >= other._ptr;
        }

        bool operator==(const iterator& other) const
        {
            return _ptr == other._ptr;
        }

        bool operator!=(const iterator& other) const
        {
            return _ptr != other._ptr;
        }
    protected:
        uint64_t* _ptr;
        size_t    _pos;
    };

    /// Constant iterator.
    struct const_iterator {
        const_iterator(uint64_t* ptr, size_t pos)
        : _ptr(ptr)
        , _pos(pos)
        {}

        const_iterator(const const_iterator&) = default;

        const_iterator& operator=(const const_iterator&) = default;

        const_iterator& operator++()
        {
            _pos++;
            return *this;
        }
        
        const_iterator operator++(int)
        {
            auto copy = *this;
            _pos++;
            return copy;
        }

        bool operator*() const
        {
            return static_cast<bool>(_ptr[_pos/QWORD_BITS] & (1 << (_pos%QWORD_BITS)));
        }

        bool operator<(const const_iterator& other) const
        {
            return _ptr < other._ptr;
        }

        bool operator<=(const const_iterator& other) const
        {
            return _ptr <= other._ptr;
        }

        bool operator>(const const_iterator& other) const
        {
            return _ptr > other._ptr;
        }

        bool operator>=(const const_iterator& other) const
        {
            return _ptr >= other._ptr;
        }

        bool operator==(const const_iterator& other) const
        {
            return _ptr == other._ptr;
        }

        bool operator!=(const const_iterator& other) const
        {
            return _ptr != other._ptr;
        }
    protected:
        uint64_t* _ptr;
        size_t    _pos;
    };

    struct comparer : ::comparer<bool>
    {
        int operator()(const bool& a, const bool& b) const override
        {
            return a - b;
        }
    };
} // detail
