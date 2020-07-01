#pragma once

#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    /// A semaphore/atomic counter.
    struct semaphore {
        static constexpr int UNBOUNDED = -1;

        /// Construct from maximum count.
        semaphore(int max = UNBOUNDED);

        semaphore(const semaphore&) = delete;

        /// Acquire the semaphore/increment the counter and return the count *after* the increment. If the current
        /// count is >= the maximum, block until it is lower. If max < 0, never block.
        int acquire();

        /// Increment the counter without acquiring it. Use this with max=1 to implement a semaphore and reference count
        /// in one.
        int increment();

        /// Release the semaphore/decrement the counter and return the count *after* the decrement.
        int release();

        /// Return the current count.
        int count() const;
    private:
        int      _max;
        spinlock _lock;
        int      _count;
    };
}}