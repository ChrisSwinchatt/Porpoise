#include <stdint.h>
#include <stddef.h>

#include <container/bitarray.hpp>
#include <memory.hpp>

#include <porpoise.hpp>
#include <porpoise/assert.hpp>
#include <porpoise/mem/heap.hpp>

extern uint8_t __kernel_end__[];

namespace porpoise { namespace mem {
    heap::fixed_heap_bin::fixed_heap_bin(uintptr_t start, size_t blksize)
    : _allocd(0)
    , _blksize(blksize)
    , _first_free(0)
    , _start(start)
    {
    }

    void heap::fixed_heap_bin::map(void* p, size_t bytes)
    {
        if (!owns(p))
        {
            return;
        }

        auto addr = reinterpret_cast<uintptr_t>(p) - _start;
        auto i = addr/_blksize + addr%_blksize;
        auto n = bytes/_blksize + bytes%_blksize;
        _bitmap.set(i, n);
    }

    bool heap::fixed_heap_bin::owns(void* p) const
    {
        return reinterpret_cast<uintptr_t>(p) >= _start && reinterpret_cast<uintptr_t>(p) < _start + total_size();
    }

    void* heap::fixed_heap_bin::allocate(size_t bytes)
    {
        PORPOISE_ASSERT_LESS_OR_EQUAL(bytes, _blksize);

        if (_allocd == _bitmap.size())
        {
            return OUT_OF_MEMORY;
        }

        check::value_incremented_scope<size_t> allocd_check_scope(_allocd, __FILE__, __LINE__, __func__);
        check::value_changed_scope<size_t> first_free_check_scope(_first_free, __FILE__, __LINE__, __func__);

        // Use cached _first_free to avoid iterating if possible.
        if (_first_free < _bitmap.size())
        {
            auto p = reinterpret_cast<void*>(_start + _first_free*_blksize);
            _bitmap.set(_first_free);
            _allocd++;

            // If the heap is not full and there have been no frees, then the block to the right is always a free block.
            // The first BLOCKS_PER_BIN allocations can therefore be performed without searching the bitmap.
            auto tmp = _first_free + 1;
            if (tmp < _bitmap.size() && _bitmap[tmp] == false)
            {
                _first_free = tmp;
            }
            else
            {
                _first_free = _bitmap.size();
            }

            return p;
        }

        for (auto i = 0UL; i < _bitmap.size(); i++)
        {
            if (!_bitmap[i])
            {
                auto p = reinterpret_cast<void*>(_start + _first_free*_blksize);
                _bitmap.set(i);
                _allocd++;

                if (!_bitmap[i + 1])
                {
                    _first_free = i;
                }

                return p;
            }
        }

        PORPOISE_UNREACHABLE("");
    }

    void* heap::fixed_heap_bin::reallocate(void* p, size_t bytes)
    {
        if (bytes <= _blksize)
        {
            return p;
        }

        return MOVE_BIN;
    }

    void heap::fixed_heap_bin::deallocate(void* p)
    {
        check::value_decremented_scope<size_t> scope(_allocd, __FILE__, __LINE__, __func__);
        auto i = (reinterpret_cast<uintptr_t>(p) - _start)/_blksize;
        _bitmap.clear(i);
        _allocd--;

        // Cache the block address if the heap is not already tracking a free block.
        if (_first_free >= _bitmap.size())
        {
            _first_free = i;
        }
    }

    size_t heap::fixed_heap_bin::total_size() const
    {
        return _blksize*_bitmap.size();
    }

    size_t heap::fixed_heap_bin::total_blocks() const
    {
        return _bitmap.size();
    }

    size_t heap::fixed_heap_bin::block_size() const
    {
        return _blksize;
    }
    
    size_t heap::fixed_heap_bin::bytes_allocated() const
    {
        return _allocd;
    }
    
    size_t heap::fixed_heap_bin::blocks_allocated() const
    {
        return _bitmap.popcount();
    }
}} // porpoise::mem
