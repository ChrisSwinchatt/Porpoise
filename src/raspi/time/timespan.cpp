#include <porpoise/time/timespan.hpp>

#define MICROS_TO_CYCLES(MICROS, HERTZ) ((MICROS)*(HERTZ)/1000000)

#define MILLIS_TO_CYCLES(MILLIS, HERTZ) ((MILLIS)*(HERTZ)/1000)

#define CYCLES_TO_MICROS(CYCLES, HERTZ) ((CYCLES)*1000000/(HERTZ))

#define CYCLES_TO_MILLIS(CYCLES, HERTZ) ((CYCLES)*1000/(HERTZ))

namespace porpoise { namespace time {
    namespace internal {
        static struct hertz_initializer {
            hertz_initializer()
            {
                uint32_t freq = 0x600DC0DE;
                asm volatile("mrs %0, cntfrq_el0":"=r"(freq));
                timespan::cpu_hertz(freq);
            }
        } instance;
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
}} // porpoise::time
