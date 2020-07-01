#include <porpoise/abort.hpp>

namespace porpoise
{
    extern "C" void abort()
    {
        while (true)
            ;
    }
}
