#pragma once

#include <porpoise/sync/basic-lock.hpp>

namespace porpoise { namespace sync {
    struct lock_guard {
        lock_guard(basic_lock& lock) : _lock(lock)
        {
            _lock.acquire();
        }

        virtual ~lock_guard()
        {
            _lock.release();
        }
    private:
        basic_lock& _lock;
    };
}} // porpoise::sync
