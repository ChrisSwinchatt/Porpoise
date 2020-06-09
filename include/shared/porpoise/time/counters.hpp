#pragma once

#include <stdint.h>

namespace porpoise { namespace time {
    struct counters {
        static uintmax_t cycles();

        static uintmax_t micros();

        static uintmax_t millis();
        
        counters() = delete;
    };
}}