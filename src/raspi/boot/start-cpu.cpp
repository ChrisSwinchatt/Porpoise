#include <stdint.h>

#include <porpoise/boot.hpp>
#include <porpoise/abort.hpp>
#include <porpoise/io/logging.hpp>

using namespace porpoise::io::logging;

namespace porpoise { namespace boot {
    void start_cpu(int id, void(* fn)())
    {
        auto source = reinterpret_cast<uintptr_t>(fn);
        switch (id)
        {
            case 1: *reinterpret_cast<uintptr_t*>(0xE0) = source; break;
            case 2: *reinterpret_cast<uintptr_t*>(0xE8) = source; break;
            case 3: *reinterpret_cast<uintptr_t*>(0xF0) = source; break;
            default:
                PORPOISE_ABORT(
                    "BUG: Got request run function "
                    << set_width(16)
                    << set_fill(0)
                    << set_base(16)
                    << show_prefix(true)
                    << source
                    << " on invalid CPU: "
                    << reset()
                    << id
                    << "; valid values are 1, 2 or 3\n"
                );
                break;
        }
    }
}} // porpoise::boot
