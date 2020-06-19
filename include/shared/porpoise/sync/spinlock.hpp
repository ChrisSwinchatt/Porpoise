#pragma once

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>

namespace porpoise { namespace sync {
    struct spinlock : basic_lock {
        spinlock();

        spinlock(spinlock&&);

        void operator=(spinlock&&);

        virtual bool acquire() override;

        virtual void release() override;

        spinlock(spinlock&) = delete;

        void operator=(spinlock&) = delete;
    private:
        atomic_flag  _lock;
        atomic<bool> _moved;
    };
}} // porpoise::sync
