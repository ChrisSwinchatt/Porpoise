#pragma once

#include <porpoise/io/logging.hpp>
#include <porpoise/io/uart.hpp>

namespace porpoise { namespace io { namespace logging {
struct serial_sink : log_sink
{
    serial_sink(uart* dev, log_level min_level);

    void emit(log_level level, const char* event);

    void emit(log_level level, char c);
private:
    uart*     _dev;
    log_level _min_level;
};
}}} // porpoise::io::logging
