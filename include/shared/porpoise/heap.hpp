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
            return_null,
            default_ = abort
        };

        static constexpr size_t DEFAULT_ALIGNMENT = sizeof(uint64_t);

        static void init();

        static void* allocate(size_t bytes);

        static void* allocate(
            size_t        bytes,
            size_t        alignment
        );

        static void* allocate(
            size_t        bytes,
            oom_behaviour behaviour
        );

        static void* allocate(
            size_t        bytes,
            size_t        alignment,
            oom_behaviour behaviour
        );
        
        static void deallocate(void* ptr);
    private:
        static constexpr uintptr_t TOP = 0x3b3fffff;
        static uintptr_t      _curr;
        static sync::spinlock _lock;

        heap() = delete;
    };
}
