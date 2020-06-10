#pragma once

#include <stdint.h>

namespace porpoise { namespace time {
    struct timespan {
        static constexpr uintmax_t DEFAULT_CPU_HERTZ = 1000000000; // 1 GHz.

        static void cpu_hertz(uintmax_t next);
    
        static uintmax_t cpu_hertz();

        static timespan get_program_counter();

        static timespan cycles(uintmax_t value);
        
        static timespan micros(uintmax_t value);
        
        static timespan millis(uintmax_t value);

        timespan(uintmax_t cycles);

        uintmax_t cycles() const;
        
        uintmax_t micros() const;
        
        uintmax_t millis() const;
    private:
        static uintmax_t _cpu_hertz;
        uintmax_t _cycles;
    };
}} // porpoise::time
