#include <porpoise/abort.hpp>

__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard* g)
{
    while (__sync_lock_test_and_set(g, 1)) {}
    return 1;
}

extern "C" void __cxa_guard_release(__guard* g)
{
    *g = 0;
}

extern "C" void __cxa_guard_abort(__guard*)
{
    PORPOISE_ABORT("__cxa_guard_abort called");
}
