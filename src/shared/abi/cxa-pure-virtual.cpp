#include <porpoise/abort.hpp>

extern "C" void __cxa_pure_virtual()
{
    PORPOISE_ABORT("Pure virtual method called");
}
