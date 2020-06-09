#include <porpoise/io/logging.hpp>
#include <porpoise/io/logging/sinks/serial-sink.hpp>
#include <porpoise/io/uart.hpp>

namespace porpoise { namespace io { namespace logging {
    serial_sink::serial_sink(uart* dev, log_level min_level)
    : _dev(dev)
    , _min_level(min_level)
    {
    }

    void serial_sink::emit(log_level level, const char* event)
    {
        if (level < _min_level)
        {
            return;
        }

        while (*event)
        {
            _dev->put(*event++);
        }
    }

    void serial_sink::emit(log_level level, char c)
    {
        if (level < _min_level)
        {
            return;
        }

        _dev->put(c);
    }
}}} // porpoise::io::logging
