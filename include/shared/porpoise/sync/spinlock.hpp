#pragma once

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>

namespace porpoise { namespace sync {
    struct spinlock : public basic_lock {
        spinlock();

        spinlock(spinlock&&);

        void operator=(spinlock&&);

        bool acquire() override;

        void release() override;

        spinlock(spinlock&) = delete;

        void operator=(spinlock&) = delete;
    private:
        atomic_flag  _lock;
        atomic<bool> _moved;
    };
}} // porpoise::sync
