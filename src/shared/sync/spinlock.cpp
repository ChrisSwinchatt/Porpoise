#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    spinlock::spinlock() : _moved(false)
    {}

    spinlock::spinlock(spinlock&& other)
    : _lock(other._lock)
    , _moved(other._moved)
    {
        other._moved.store(true);
    }

    void spinlock::operator=(spinlock&& other)
    {
        _moved = other._moved;
        _lock  = other._lock;
        other._moved.store(true);
    }

    bool spinlock::acquire()
    {
        if (_moved)
        {
            return false;
        }

#if CONFIG_SMP_ENABLED
        while (_lock.test_and_set())
        {
            // Do nothing.
        }
#endif

        return true;
    }

    void spinlock::release()
    {
        if (!_moved)
        {
            _lock.clear();
        }
    }
}} // porpoise::sync
