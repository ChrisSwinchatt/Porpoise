#pragma once

#include <porpoise/sync/atomic.hpp>
#include <porpoise/sync/basic-lock.hpp>

namespace porpoise { namespace sync {
    struct spinlock : public basic_lock {
        bool acquire() final
        {
            while (_flag.test_and_set())
            {
                // Do nothing.
            }

            return true;
        }

        void release() final
        {
            _flag.clear();
        }
    private:
        porpoise::sync::atomic_flag _flag;
    };
}} // porpoise::sync
