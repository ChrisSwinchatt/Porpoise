#include <porpoise/time/config.hpp>
#include <porpoise/time/counters.hpp>

namespace porpoise { namespace time {
    uintmax_t counters::cycles()
    {
        uint64_t count = 0xdeadbeef;
        asm volatile(
            "\tisb\n"
            "\tmrs %0, pmccntr_el0\n":"=r"(count)
        );
        return count;
    }

    uintmax_t counters::micros()
    {
        return cycles()*config::cpu_hertz()/1000000;
    }

    uintmax_t counters::millis()
    {
        return cycles()*config::cpu_hertz()/1000;
    }
}}
