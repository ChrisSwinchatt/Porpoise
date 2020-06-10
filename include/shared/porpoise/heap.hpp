#pragma once

#include <stdint.h>
#include <stddef.h>

#include <porpoise/sync/spinlock.hpp>

namespace porpoise {
    struct heap
    {
        enum class oom_behaviour
        {
            abort,
            null
        };

        static void init();

        static void* allocate(
            size_t        bytes,
            size_t        alignment = sizeof(uint64_t),
            oom_behaviour behaviour = oom_behaviour::abort
        );
        
        static void deallocate(void* ptr);
    private:
        static constexpr uintptr_t TOP = 0x3b3fffff;
        static uintptr_t      _curr;
        static sync::spinlock _lock;

        heap() = delete;
    };
}
