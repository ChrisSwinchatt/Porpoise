#include <porpoise.hpp>
#include <porpoise/boot.hpp>
#include <porpoise/heap.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/io/logging/sinks/serial-sink.hpp>
#include <porpoise/io/uart.hpp>
#include <porpoise/sync/atomic.hpp>
#include <porpoise/time/timespan.hpp>
#include <porpoise/time/delay.hpp>

using namespace porpoise::io;
using namespace porpoise::io::logging;

extern "C" void _start();
extern "C" void _init();
extern "C" void _fini();

extern "C" uint8_t __bss_start__[];
extern "C" uint8_t __bss_end__[];

namespace porpoise { namespace boot {
    using sync::atomic;
    using time::delay;
    using time::timespan;

    static atomic<bool> cpu0_init_complete(false);

    static void clear_bss()
    {
        auto p = reinterpret_cast<uint8_t*>(__bss_start__);
        while (p < __bss_end__)
        {
            *p++ = 0;
        }
    }

    // Ensure the other 3 CPUs run.
    static void ensure_other_cpus()
    {
        start_cpu(1, _start);
        start_cpu(2, _start);
        start_cpu(3, _start);
    }

    static void start_cpu_0(uint64_t dtb_address)
    {
        PORPOISE_UNUSED(dtb_address);
        clear_bss();
        _init();

        heap::init();
        static uart*       uart0 = uart::init(uart::baud_rate::bd115200);
        static serial_sink uart0_sink(uart0, log_level::trace);
        log::add_sink(&uart0_sink);

        ensure_other_cpus();

        cpu0_init_complete.store(true);

        cpu_main(0);

        _fini();
    }

    static void start_cpu(int id)
    {
        // Wait for CPU 0 initialisation.
        while (!cpu0_init_complete.load())
        {
            delay(timespan::millis(1));
        }

        cpu_main(id);
    }

    extern "C" void boot_kernel(uint64_t dtb_address, uint64_t id)
    {
        PORPOISE_UNUSED(dtb_address);
        if (id == 0)
        {
            start_cpu_0(dtb_address);
        }
        else
        {
            start_cpu(id);
        }

        while (true)
            ;
    }
}} // porpoise::boot
