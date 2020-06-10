#include <stdint.h>
#include <stddef.h>

#include <porpoise.hpp>
#include <porpoise/heap.hpp>
#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

extern uint8_t __kernel_end__[];

namespace porpoise {
    uintptr_t heap::_curr;
    sync::spinlock heap::_lock;

    void heap::init()
    {
        _curr = reinterpret_cast<uintptr_t>(__kernel_end__);
    }

    void* heap::allocate(size_t bytes, size_t alignment, oom_behaviour behaviour)
    {
        sync::lock_guard guard(_lock);

        _curr += _curr & ~(alignment - 1);
        if (_curr + bytes > TOP)
        {
            if (behaviour == oom_behaviour::abort)
            {
                PORPOISE_ABORT("Out of memory");
            }

            return nullptr;
        }

        auto p = _curr;
        _curr += bytes;
        return reinterpret_cast<void*>(p);
    }
    
    void heap::deallocate(void* ptr)
    {
        PORPOISE_UNUSED(ptr);
    }
}

void* operator new(size_t size)
{
    return porpoise::heap::allocate(size);
}

void* operator new(size_t size, void* ptr)
{
    PORPOISE_UNUSED(size);
    return ptr;
}

void operator delete(void* ptr)
{
    porpoise::heap::deallocate(ptr);
}

void operator delete(void* ptr, size_t size)
{
    PORPOISE_UNUSED(size);
    porpoise::heap::deallocate(ptr);
}
