#pragma once

#include <stdint.h>

namespace porpoise { namespace time {
    struct config
    {
        static constexpr uintmax_t DEFAULT_CPU_HERTZ = 1000000000; // 1 GHz.

        static void cpu_hertz(uintmax_t next);
    
        static uintmax_t cpu_hertz();
    private:
        static uintmax_t _cpu_hertz;

        config() = delete;
    };
}} // porpose::time
