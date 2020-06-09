#pragma once

#include <stdint.h>
#include <stddef.h>

namespace porpoise {
    struct heap
    {
        static void init();

        static void* allocate(size_t bytes);
        
        static void deallocate(void* ptr);
    private:
        static constexpr uintptr_t TOP = 0x3b3fffff;
        static uintptr_t curr;

        heap() = delete;
    };
}
