#pragma once

#include <stdint.h>

#include <move.hpp>

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    struct lock_guard {
        explicit lock_guard(spinlock& lock);

        virtual ~lock_guard();

        lock_guard(lock_guard&& other);

        void operator=(lock_guard&& other);

        lock_guard(lock_guard& other) = delete;
        void operator=(lock_guard& other) = delete;
    private:
        spinlock* _lock;
    };
}} // porpoise::sync
