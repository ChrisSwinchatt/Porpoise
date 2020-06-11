#include <stdint.h>

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>
#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    lock_guard::lock_guard(spinlock& lock) : _lock(&lock)
    {
        _lock->acquire();
    }

    lock_guard::~lock_guard()
    {
        if (_lock != nullptr)
        {
            _lock->release();
        }
    }

   lock_guard::lock_guard(lock_guard&& other)
    {
        _lock        = other._lock;
        other._lock  = nullptr;
    }

    void lock_guard::operator=(lock_guard&& other)
    {
        _lock        = other._lock;
        other._lock  = nullptr;
    }
}} // porpoise::sync
