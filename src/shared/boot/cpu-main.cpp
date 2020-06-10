#include <porpoise/boot.hpp>
#include <porpoise/io/logging.hpp>

using namespace porpoise::io::logging;

namespace porpoise { namespace boot {
    void cpu_main(int id)
    {
        PORPOISE_LOG_INFO("Hello from CPU " << id << '!');
    }
}} // porpoise::boot
