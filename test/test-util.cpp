#include <cstdlib>
#include <ctime>
#include <iostream>

#include <porpoise.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/time/timespan.hpp>
#include <porpoise/time/delay.hpp>

#include "test-util.hpp"

using namespace porpoise::io::logging;

static struct cerr_sink : log_sink
{
    /// Emit a string verbatim if the log level is greater than or equal to the sink's log-level.
    void emit(log_level level, const char* event) override
    {
        PORPOISE_UNUSED(level);
        std::cerr << event;
    }

    /// Emit a character verbatim if the log level is greater than or equal to the sink's log-level.
    void emit(log_level level, char c) override
    {
        PORPOISE_UNUSED(level);
        std::cerr << c;
    }
} _cerr_sink;

static struct test_initializer {
    test_initializer()
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        log::add_sink(&_cerr_sink);
    }
} _initializer;

namespace porpoise { namespace time {
    timespan timespan::get_program_counter()
    {
        return timespan::micros(0);
    }

    void delay(const timespan& t)
    {
        PORPOISE_UNUSED(t);
    }
}}
