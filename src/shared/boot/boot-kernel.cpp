#include <porpoise.hpp>
#include <porpoise/boot.hpp>
#include <porpoise/capability.hpp>
#include <porpoise/mem/heap.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/io/logging/sinks/serial-sink.hpp>
#include <porpoise/io/uart.hpp>
#include <porpoise/sync/atomic.hpp>
#include <porpoise/time/timespan.hpp>
#include <porpoise/time/delay.hpp>

using namespace porpoise::io;
using namespace porpoise::io::logging;
using namespace porpoise::sync;
using namespace porpoise::time;

extern "C" void _start();
extern "C" void _init();
extern "C" void _fini();

extern "C" uint8_t __bss_start__[];
extern "C" uint8_t __bss_end__[];
extern "C" uint8_t __kernel_end__[];

namespace porpoise {
    static struct init_capability {} __init_capability;
    static struct heap_capability {} __heap_capability;
}

namespace porpoise { namespace boot {
    static atomic<bool> cpu0_init_complete(false);
    static atomic<int> cpus_started(false);

    static void clear_bss()
    {
        auto p = reinterpret_cast<uint8_t*>(__bss_start__);
        while (p < __bss_end__)
        {
            *p++ = 0;
        }
    }

#if CONFIG_SMP_ENABLED
    // Ensure the other 3 CPUs run.
    static void ensure_other_cpus()
    {
        start_cpu(1, _start);
        start_cpu(2, _start);
        start_cpu(3, _start);
    }
#endif

    static bool is_ptr_null(void* ptr)
    {
        if (ptr == nullptr)
        {
            PORPOISE_LOG_ERROR("Heap returned null pointer");
            return true;
        }

        return false;
    }

    static bool is_ptr_unaligned(void* ptr)
    {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        if (mem::is_address_aligned(addr, sizeof(uint64_t)))
        {
            return false;
        }

        PORPOISE_LOG_ERROR("Heap returned misaligned address: " << ptr);
        return true;
    }

    static size_t check_heap_ptrs(mem::heap& heap, size_t blksize)
    {
        size_t checks_failed = 0;
        auto p = heap.allocate(blksize, mem::oom_behaviour::abort);
        auto q = heap.allocate(blksize, mem::oom_behaviour::abort);

        if (is_ptr_null(p))
        {
            checks_failed++;
        }
        else if (is_ptr_unaligned(p))
        {
            checks_failed++;
        }

        if (is_ptr_null(q))
        {
            checks_failed++;
        }
        else if (is_ptr_unaligned(q))
        {
            checks_failed++;
        }

        heap.deallocate(p);
        heap.deallocate(q);
        return checks_failed;
    }

    static mem::heap& bring_up_heap(void* dtb_address)
    {
        auto& heap = mem::heap::get(__heap_capability);
        heap.init(__init_capability, reinterpret_cast<uintptr_t>(__kernel_end__));

        // Map the DTB if present.
        if (dtb_address == nullptr)
        {
            PORPOISE_LOG_WARN("Didn't receive DTB, might not be able to bring up devices");
        }
        else
        {
            heap.map(dtb_address, 0x1000);
        }

        // Sanity check the heap's behaviour.
        // * It should not return nullptrs (with oom_behaviour::abort)
        // * It should not return a pointer that is already allocated
        // * The addresses returned should be aligned to 8 byte boundaries
        size_t checks_failed = 0;
        for (
            size_t i = 0, blksize = mem::heap::MIN_BIN_BLOCK_SIZE, grow = 2;
            i <= mem::heap::TOTAL_NUM_BIN;
            i++, blksize *= grow, grow <<= 1
        )
        {
            checks_failed += check_heap_ptrs(heap, blksize);
        }

        if (checks_failed > 0)
        {
            PORPOISE_ABORT("Heap failed " << checks_failed << " sanity checks");
        }

        return heap;
    }

    // CPU 0 brings up subsystems.
    static void start_cpu_0(void* dtb_address)
    {
        PORPOISE_UNUSED(dtb_address);
        clear_bss();
        _init();

        cpus_started++;

        static uart* uart0 = uart::init(uart::baud_rate::bd115200);
        static serial_sink uart0_sink(uart0, log_level::trace);
        log::add_sink(&uart0_sink);

        PORPOISE_LOG_DEBUG("CPU 0 started (dtb=" << dtb_address << "), starting basic initialization");

        auto& heap = bring_up_heap(dtb_address);
        PORPOISE_UNUSED(heap);

#if CONFIG_SMP_ENABLED
        PORPOISE_LOG_DEBUG("CPU 0 initialization complete, waking up remaining CPUs");
        ensure_other_cpus();
#else
        PORPOISE_LOG_DEBUG("CPU 0 initialization complete, entering cpu_main [SMP disabled]");
#endif

        cpu0_init_complete.store(true);
        cpu_main(0, __heap_capability);

        _fini();
    }

    // Remaining CPUs wait for CPU0 to finish initialising.
    static void start_cpu(int id)
    {
        delay(timespan::millis(100));
        cpus_started++;

        // Wait for CPU 0 initialisation.
        while (!cpu0_init_complete.load())
        {
            delay(timespan::millis(1));
        }

        cpu_main(id, __heap_capability);
    }

    extern "C" void boot_kernel(uint64_t dtb_address, uint64_t id)
    {
        if (id == 0)
        {
            dtb_address &= 0x00000000FFFFFFFF; // Clear upper 32 bits.
            start_cpu_0(reinterpret_cast<void*>(dtb_address));
        }
        else
        {
            start_cpu(static_cast<int>(id));
        }

        while (true)
            ;
    }
}} // porpoise::boot
