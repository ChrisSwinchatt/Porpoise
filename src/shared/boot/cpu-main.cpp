#include <porpoise.hpp>
#include <porpoise/boot.hpp>
#include <porpoise/capability.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/time/timespan.hpp>

using namespace porpoise::io::logging;

namespace porpoise { namespace boot {
    void cpu_main(int id, const heap_capability& heap_cap)
    {
        PORPOISE_UNUSED(heap_cap);
        PORPOISE_LOG_INFO("Hello from CPU " << id << " (" << time::timespan::cpu_hertz()/1000000 << "MHz)");
    }
}} // porpoise::boot
