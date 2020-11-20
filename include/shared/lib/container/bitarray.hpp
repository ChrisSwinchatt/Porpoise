#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include <algorithm.hpp>
#include <container/array.hpp>

#include <container/detail/bitarray-detail.hpp>

/// A fixed-size bitarray that packs bits into qwords.
/// Bits are initialised to 0 by default but can be explicitly initialised to 1.
/// This container is *NOT* threadsafe.
template <size_t N>
struct bitarray {
    using reference      = bitarray_detail::reference;
    using iterator       = bitarray_detail::iterator;
    using const_iterator = bitarray_detail::const_iterator;

    static constexpr size_t QWORD_BITS = bitarray_detail::QWORD_BITS;
    static constexpr size_t QWORDS     = N/QWORD_BITS + N%QWORD_BITS;

    /// Initialise with zeros.
    bitarray()
    : _bits(0)
    , _dirty(true)
    , _last_popcount(0)
    {
    }

    /// Initialise with value.
    explicit bitarray(bool value)
    : _bits(value ? UINT64_MAX : 0)
    , _dirty(true)
    , _last_popcount(0)
    {
    }

    /// Get iterator to first set bit.
    const_iterator find() const
    {
        return linear_search(true, begin(), end(), bitarray_detail::comparer());
    }
    
    iterator find()
    {
        return linear_search(true, begin(), end(), bitarray_detail::comparer());
    }

    /// Get iterator to first bit with specified value.
    const_iterator find(bool value) const
    {
        return linear_search(value, begin(), end(), bitarray_detail::comparer());
    }

    iterator find(bool value)
    {
        return linear_search(value, begin(), end(), bitarray_detail::comparer());
    }

    /// Get iterator to first set bit, starting at start_pos.
    iterator find(iterator start_pos)
    {
        return linear_search(true, start_pos, end(), bitarray_detail::comparer());
    }

    /// Get iterator to first set bit, starting at start_pos.
    const_iterator find(const_iterator start_pos) const
    {
        return linear_search(true, start_pos, end(), bitarray_detail::comparer());
    }

    /// Get iterator to first bit with specified value, starting at than start_pos.
    const_iterator find(const_iterator start_pos, bool value) const
    {
        return linear_search(value, start_pos, end(), bitarray_detail::comparer());
    }

    iterator find(iterator start_pos, bool value)
    {
        return linear_search(value, start_pos, end(), bitarray_detail::comparer());
    }

    /// Get iterator to the first bit.
    iterator begin()
    {
        _dirty = true;
        return iterator(&_bits[0], 0);
    }

    /// Get const iterator to the first bit.
    const iterator begin() const
    {
        return iterator(&_bits[0], 0);
    }

    /// Get iterator to the last bit.
    iterator end()
    {
        _dirty = true;
        return iterator(&_bits[0], N);
    }

    /// Get const iterator to the last bit.
    const iterator end() const
    {
        return iterator(&_bits[0], N);
    }

    /// Get itererator to the bit at pos.
    iterator at(size_t pos)
    {
        return iterator(&_bits[0], pos);
    }

    /// Set the bit at i.
    void set(size_t i)
    {
        _dirty = true;
        auto mask = 1 << (i%QWORD_BITS);
        if ((_bits[i/QWORD_BITS] & mask) == 0)
        {
            _bits[i/QWORD_BITS] |= mask;
        }
    }

    /// Set n bits starting from i. May be more efficient than iterating manually when bits are split across multiple
    /// qwords.
    void set(size_t i, size_t n)
    {
        if (n == 0)
        {
            return;
        }
        
        if (n == 1)
        {
            set(i);
            return;
        }

        _dirty = true;
        // Set bits in the first block.
        auto block = i/QWORD_BITS;
        for (; i < QWORD_BITS && n > 0; i++, n--)
        {
            _bits[block] |= 1 << (i%QWORD_BITS);
        }

        // Set bits in any middle block(s).
        for (; n >= QWORD_BITS; block++, n -= QWORD_BITS)
        {
            _bits[block] = UINT64_MAX;
        }

        // Set bits in the last block.
        while (n--)
        {
            _bits[block] |= 1 << (n%QWORD_BITS);
        }
    }

    /// Clear the bit at i.
    void clear(size_t i)
    {
        _dirty = true;
        auto mask = 1 << (i%QWORD_BITS);
        if ((_bits[i/QWORD_BITS] & mask) != 0)
        {
            _bits[i/QWORD_BITS] &= ~mask;
        }
    }

    /// Clear n bits starting from i.
    void clear(size_t i, size_t n)
    {
        if (n == 0)
        {
            return;
        }
        
        if (n == 1)
        {
            set(i);
            return;
        }

        _dirty = true;
        // Clear bits in the first block.
        auto block = i/QWORD_BITS;
        for (; i < QWORD_BITS && n > 0; i++, n--)
        {
            _bits[block] &= ~(1 << (i%QWORD_BITS));
        }

        // Clear bits in any middle block(s).
        for (; n >= QWORD_BITS; block++, n -= QWORD_BITS)
        {
            _bits[block] = 0;
        }

        // Clear bits in the last block.
        while (n--)
        {
            _bits[block] &= ~(1 << (n%QWORD_BITS));
        }
    }

    /// Toggle the bit at i.
    void toggle(size_t i)
    {
        _dirty = true;
        auto mask = 1 << (i%QWORD_BITS);
        if ((_bits[i/QWORD_BITS] & mask))
        {
            _bits[i/QWORD_BITS] &= ~mask;
        }
        else
        {
            _bits[i/QWORD_BITS] |= mask;
        }
    }

    /// Get the bit at index.
    bool operator[](size_t index) const
    {
        return _bits[index/QWORD_BITS] & (1 << index%QWORD_BITS);
    }

    /// Get a reference to the bit at index.
    reference operator[](size_t index)
    {
        _dirty = true;
        return reference(_bits[index/QWORD_BITS], 1 << (index%QWORD_BITS));
    }

    /// Get the total number of bits in the bitarray.
    size_t size() const
    {
        return N;
    }

    /// Get the number of set bits in the bitarray.
    size_t popcount() const
    {
        // if (_dirty)
        {
            return internal_popcount();
        }

        return _last_popcount;
    }

    /// Get the number of set bits in the bitarray.
    size_t popcount()
    {
        // if (_dirty)
        {
            _last_popcount = internal_popcount();
            _dirty = false;
        }

        return _last_popcount;
    }
private:
    array<uint64_t, QWORDS> _bits;
    bool                    _dirty;
    size_t                  _last_popcount;

    size_t internal_popcount() const
    {
        auto count = 0UL;
        for (auto qword : _bits)
        {
            count += __builtin_popcountll(qword);
        }
        return count;
    }
};
