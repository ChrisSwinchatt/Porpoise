#include <stdint.h>
#include <stddef.h>

#include <algorithm.hpp>
#include <container/array.hpp>
#include <container/bitarray.hpp>
#include <memory.hpp>

#include <porpoise.hpp>
#include <porpoise/assert.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/mem/heap.hpp>
#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

using namespace porpoise::sync;

extern uint8_t __kernel_end__[];

void* operator new(size_t size, void* p)
{
    PORPOISE_UNUSED(size);
    return p;
}

namespace porpoise { namespace mem {
    heap heap::_inst;

    bool is_address_aligned(uintptr_t address, size_t boundary)
    {
        return (address & (boundary - 1)) == 0;
    }

    uintptr_t get_aligned_address(uintptr_t address, size_t boundary)
    {
        return address & ~(boundary - 1);
    }

    ptrdiff_t make_address_aligned(uintptr_t& address, size_t boundary)
    {
        auto next = get_aligned_address(address, boundary);
        auto diff = address - next;
        address = next;
        return diff;
    }

    heap& heap::get(const heap_capability& cap)
    {
        PORPOISE_UNUSED(cap);
        return _inst;
    }

    void heap::init(const init_capability& cap, uintptr_t start)
    {
        PORPOISE_UNUSED(cap);
        lock_guard guard(_lock);

        // Ensure address is 8-byte aligned.
        _start = get_aligned_address(start, sizeof(uint64_t));
        _end   = _start;
        PORPOISE_ASSERT_EQUAL(_start & (sizeof(uint64_t) - 1), 0UL);

        // Allocate space for the bin objects.
        auto bin_objs = reinterpret_cast<heap_bin*>(_end);
        _end += NUM_FIXED_BIN*sizeof(fixed_heap_bin) + sizeof(variable_heap_bin);

        // Allocate the area managed by each fixed-size bin. Each bin[i] has a blksize[i] of blksize[i - 1]*2^i, so if
        // the minimum block size is 16, the next is 32, then 128, etc.
        size_t blksize = MIN_BIN_BLOCK_SIZE;
        size_t blksize_grow = 2;
        size_t i;
        for (i = 0;
             i < _bins.size() - 1 && (MAX_BIN_BLOCK_SIZE == 0 || blksize < MAX_BIN_BLOCK_SIZE);
             i++, blksize *= blksize_grow, blksize_grow <<= 1)
        {
            _bins[i] = new (bin_objs) fixed_heap_bin(_end, blksize);
            _end += bin_objs->total_size();
            bin_objs = reinterpret_cast<heap_bin*>(reinterpret_cast<uint8_t*>(bin_objs) + sizeof(fixed_heap_bin));
        }

        auto fixed_total_size = _end - _start;
        auto variable_size = MAX_HEAP_SIZE - fixed_total_size;

        // Allocate the area for the variable bin.
        _bins[i] = new (bin_objs) variable_heap_bin(_end, variable_size);
        _end += bin_objs->total_size();

        PORPOISE_ASSERT_GREATER(_end, _start);

        PORPOISE_LOG_DEBUG(
            "Initialised heap of "
            << (_end - _start) 
            << " B at " 
            << reinterpret_cast<void*>(_start)
            << " with "
            << fixed_total_size
            << " B in fixed bins, and "
            << variable_size
            << " B in variable bin"
        );
    }

    void heap::map(void* p, size_t size)
    {
        PORPOISE_CHECK_UNEQUAL(p, nullptr);

        for (auto bin : _bins)
        {
            if (bin->owns(p))
            {
                bin->map(p, size);
            }
        }

        PORPOISE_LOG_TRACE("Mapped " << size << " B object at " << p);
    }

    void* heap::handle_oom(oom_behaviour behaviour)
    {
        if (behaviour == oom_behaviour::abort)
        {
            PORPOISE_ABORT("Out of memory");
        }

        PORPOISE_LOG_WARN("Returning NULL for OOM event");
        return OUT_OF_MEMORY;
    }

    void* heap::allocate(size_t bytes, oom_behaviour behaviour)
    {
        PORPOISE_CHECK_UNEQUAL(bytes, 0UL);
        lock_guard guard(_lock);
        for (auto bin : _bins)
        {
            if (bytes <= bin->block_size())
            {
                auto p = bin->allocate(bytes);
                if (p != OUT_OF_MEMORY)
                {
                    PORPOISE_LOG_TRACE(
                        "Allocated "
                        << bytes
                        << " B at "
                        << p
                        << " using "
                        << (bin->is_variable_heap() ? "variable" : "fixed")
                        << " bin (blksize="
                        << bin->block_size()
                        << ")"
                    );
                    _allocations++;
                    return p;
                }
            }
        }

        return handle_oom(behaviour);
    }
    
    void* heap::reallocate(void* p, size_t bytes, oom_behaviour behaviour)
    {
        PORPOISE_CHECK_UNEQUAL(p, nullptr);
        lock_guard guard(_lock);
        for (auto bin : _bins)
        {
            if (bin->owns(p))
            {
                // Fixed size bins return nullptr when a resized pointer should move bin. The variable sized bin returns
                // nullptr when reallocation fails due to OOM, unless bytes is zero, in which case NULL means p was
                // deallocated.
                auto q = bin->reallocate(p, bytes);
                if (q != nullptr)
                {
                    PORPOISE_LOG_TRACE(
                        "Reallocated "
                        << bytes
                        << " B object at "
                        << p
                        << " in place using "
                        << (bin->is_variable_heap() ? "variable" : "fixed")
                        << " bin (blksize="
                        << bin->block_size()
                        << ")"
                    );

                    return q;
                }

                if (bin->is_variable_heap() && bytes > 0)
                {
                    return handle_oom(behaviour);
                }

                PORPOISE_LOG_TRACE(
                    "Moving "
                    << bytes
                    << " B object at "
                    << p
                    << " using "
                    << (bin->is_variable_heap() ? "variable" : "fixed")
                    << " bin (blksize="
                    << bin->block_size()
                    << ")"
                );

                return allocate(bytes, behaviour);
            }
        }

        return handle_oom(behaviour);
    }

    void heap::deallocate(void* p)
    {
        PORPOISE_CHECK_UNEQUAL(p, nullptr);
        lock_guard guard(_lock);
        for (auto bin : _bins)
        {
            if (bin->owns(p))
            {
                PORPOISE_LOG_TRACE(
                    "Freed object at "
                    << p
                    << " from "
                    << (bin->is_variable_heap() ? "variable" : "fixed")
                    << " bin (blksize="
                    << bin->block_size()
                    << ")"
                );

                bin->deallocate(p);
                _frees++;
                return;
            }
        }

        PORPOISE_LOG_WARN("Got request to free object at " << p << " but it is not owned by any bin");
    }

    void heap::get_statistics(statistics& stats)
    {
        zero_object(stats);
        stats.num_bins = _bins.size();
        stats.allocations = _allocations;
        stats.frees = _frees;
        for (auto bin : _bins)
        {
            stats.total_bytes += bin->total_size();
            stats.bytes_allocated += bin->bytes_allocated();
            stats.total_blocks += bin->total_blocks();
            stats.blocks_allocated += bin->blocks_allocated();
        }
    }
}} // porpoise::mem

void* operator new(size_t size)
{
    PORPOISE_UNUSED(size);
    PORPOISE_ABORT("new called");
}

void* operator new[](size_t size)
{
    PORPOISE_UNUSED(size);
    PORPOISE_ABORT("new called");
}

void operator delete(void* p)
{
    PORPOISE_UNUSED(p);
    PORPOISE_ABORT("new called");
}

void operator delete[](void* p)
{
    PORPOISE_UNUSED(p);
    PORPOISE_ABORT("new called");
}

void operator delete[](void* p, size_t size)
{
    PORPOISE_UNUSED(p);
    PORPOISE_UNUSED(size);
    PORPOISE_ABORT("new called");
}

void operator delete(void* p, size_t size)
{
    PORPOISE_UNUSED(p);
    PORPOISE_UNUSED(size);
    PORPOISE_ABORT("new called");
}
