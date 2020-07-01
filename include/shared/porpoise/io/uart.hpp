#pragma once

#include <stdint.h>

namespace porpoise { namespace io {
    struct uart {
        enum class baud_rate
        {
            bd9600,
            bd115200
        };

        static uart* init(baud_rate baud);

        void put(uint8_t value);

        uint8_t get();
    private:
        static uart _instance;
        baud_rate _baud;

        uart() = default;
    };
}} // porpoise::io
