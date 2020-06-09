#include <stdint.h>
#include <porpoise/time/config.hpp>
#include <porpoise/time/delay.hpp>

namespace porpoise { namespace time {
    void delay::cycles(uintmax_t count)
    {
        asm volatile(
            "\t__delay_%=: subs %[count], %[count], #1\n\tbne __delay_%=\n"
            :"=r"(count): [count]"0"(count) : "cc"
        );
    }

    void delay::micros(uintmax_t count)
    {
        cycles(count*config::cpu_hertz()/1000000);
    }

    void delay::millis(uintmax_t count)
    {
        cycles(count*config::cpu_hertz()/1000);
    }
}} // porpose::time
