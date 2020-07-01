#include <porpoise/boot.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/time/timespan.hpp>

using namespace porpoise::io::logging;

namespace porpoise { namespace boot {
    void cpu_main(int id)
    {
        PORPOISE_LOG_INFO("Hello from CPU " << id << " (" << time::timespan::cpu_hertz()/1000000 << "MHz)");
    }
}} // porpoise::boot
