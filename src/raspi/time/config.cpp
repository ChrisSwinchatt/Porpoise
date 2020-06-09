#include <stdint.h>

#include <porpoise/time/config.hpp>

namespace porpoise { namespace time {
    uintmax_t config::_cpu_hertz;

    void config::cpu_hertz(uintmax_t next)
    {
        _cpu_hertz = next;
    }

    uintmax_t config::cpu_hertz()
    {
        return _cpu_hertz;
    }
}} // porpose::time
