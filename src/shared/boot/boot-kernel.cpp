#include <porpoise.hpp>
#include <porpoise/boot.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/io/logging/sinks/serial-sink.hpp>
#include <porpoise/io/uart.hpp>
#include <porpoise/sync/atomic.hpp>
#include <porpoise/time/timespan.hpp>
#include <porpoise/time/delay.hpp>

using namespace porpoise::io;
using namespace porpoise::io::logging;

extern "C" void _init();
extern "C" void _fini();

extern "C" uint8_t __bss_start__[];
extern "C" uint8_t __bss_end__[];

static void clear_bss()
{
    auto p = reinterpret_cast<uint8_t*>(__bss_start__);
    while (p < __bss_end__)
    {
        *p++ = 0;
    }
}

namespace porpoise { namespace boot {
    using sync::atomic;
    using time::delay;
    using time::timespan;

    static atomic<int> cpu_id(0);

    extern "C" void boot_kernel(uint64_t dtb_address)
    {
        PORPOISE_UNUSED(dtb_address);
        auto me = cpu_id++;
        if (me == 0)
        {
            clear_bss();
            _init();

            static uart*       uart0 = uart::init(uart::baud_rate::bd9600);
            static serial_sink uart0_sink(uart0, log_level::trace);
            log::add_sink(&uart0_sink);
        }
        else
        {
            delay(timespan::millis(1000));
        }

        cpu_main(me);

        if (me == 0)
        {
            _fini();
        }

        while (true)
            ;
    }
}} // porpoise::boot