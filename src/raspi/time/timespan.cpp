#include <porpoise/time/timespan.hpp>

#define MICROS_TO_CYCLES(MICROS, HERTZ) ((MICROS)*(HERTZ)/1000000)

#define MILLIS_TO_CYCLES(MILLIS, HERTZ) ((MILLIS)*(HERTZ)/1000)

#define CYCLES_TO_MICROS(CYCLES, HERTZ) ((CYCLES)*1000000/(HERTZ))

#define CYCLES_TO_MILLIS(CYCLES, HERTZ) ((CYCLES)*1000/(HERTZ))

namespace porpoise { namespace time {
    struct hertz_setter
    {
        hertz_setter()
        {
            uint32_t freq;
            timespan::cpu_hertz(freq);
        }
    } set_hertz;

    uintmax_t timespan::_cpu_hertz = DEFAULT_CPU_HERTZ;

    void timespan::cpu_hertz(uintmax_t next)
    {
        _cpu_hertz = next;
    }
    
    uintmax_t timespan::cpu_hertz()
    {
        return _cpu_hertz;
    }

    timespan timespan::get_program_counter()
    {
        uint64_t count = 0xdeadbeef;
        asm volatile(
            "\tisb\n"
            "\tmrs %0, pmccntr_el0\n":"=r"(count)
        );
        return cycles(count);
    }

    timespan timespan::cycles(uintmax_t value)
    {
        return timespan(value);
    }
        
    timespan timespan::micros(uintmax_t value)
    {
        return timespan(MICROS_TO_CYCLES(value, _cpu_hertz));
    }
        
    timespan timespan::millis(uintmax_t value)
    {
        return timespan(MILLIS_TO_CYCLES(value, _cpu_hertz));
    }

    timespan::timespan(uintmax_t cycles) : _cycles(cycles)
    {
    }

    uintmax_t timespan::cycles() const
    {
        return _cycles;
    }
        
    uintmax_t timespan::micros() const
    {
        return CYCLES_TO_MICROS(_cycles, _cpu_hertz);
    }
        
    uintmax_t timespan::millis() const
    {
        return CYCLES_TO_MILLIS(_cycles, _cpu_hertz);
    }
}} // porpoise::time
