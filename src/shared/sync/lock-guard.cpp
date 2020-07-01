#include <stdint.h>

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>
#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    lock_guard::lock_guard(basic_lock& lock) : _lock(&lock)
    {
        _lock->acquire();
    }

    lock_guard::~lock_guard()
    {
        _lock->release();
    }
}} // porpoise::sync
