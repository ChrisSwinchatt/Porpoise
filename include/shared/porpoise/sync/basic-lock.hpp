#pragma once

namespace porpoise { namespace sync {
    /// Interface implemented by locks.
    struct basic_lock {
        /// Acquire the lock, potentially a blocking call.
        virtual bool acquire() = 0;

        /// Release the lock.
        virtual void release() = 0;

        basic_lock(basic_lock&) = delete;
        void operator=(basic_lock&) = delete;
    protected:
        basic_lock() = default;
    };
}} // porpoise::sync
