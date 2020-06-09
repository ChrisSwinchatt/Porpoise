#pragma once

#include <stdint.h>

namespace porpoise { namespace time {
    struct delay
   {
        static void cycles(uintmax_t count);

        static void micros(uintmax_t count);

        static void millis(uintmax_t count);
    private:
        delay() = delete;
    };
}} // porpose::time
