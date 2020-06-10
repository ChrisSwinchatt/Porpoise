#include <stdint.h>
#include <stddef.h>

#include <porpoise.hpp>
#include <porpoise/heap.hpp>

extern uint8_t __kernel_end__[];

namespace porpoise {
    uintptr_t heap::curr;

    void heap::init()
    {
        curr = reinterpret_cast<uintptr_t>(__kernel_end__);
    }

    void* heap::allocate(size_t bytes)
    {
        curr += curr & ~(sizeof(uint64_t) - 1);
        auto p = curr;
        curr += bytes;
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
