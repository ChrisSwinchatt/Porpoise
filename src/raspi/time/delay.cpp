#include <stdint.h>
#include <porpoise/time/delay.hpp>

namespace porpoise { namespace time {
    void delay(const timespan& t)
    {
        auto count = t.cycles();
        asm volatile(
            "\t__delay_%=: subs %[count], %[count], #1\n\tbne __delay_%=\n"
            :"=r"(count): [count]"0"(count) : "cc"
        );
    }
}} // porpose::time
