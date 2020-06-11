#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/semaphore.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace sync {
    semaphore::semaphore(int max)
    : _max(max)
    , _count(0)
    {
    }

    int semaphore::acquire()
    {
        while (true)
        {
            lock_guard guard(_lock);
            if (_max < 0 || _count < _max)
            {
                return ++_count;
            }
        }
    }

    int semaphore::increment()
    {
        lock_guard guard(_lock);
        return ++_count;
    }

    int semaphore::release()
    {
        lock_guard guard(_lock);
        _count--;
        if (_count < 0)
        {
            _count = 0;
        }

        return _count;
    }

    int semaphore::count() const
    {
        return _count;
    }
}} // porpoise::sync
