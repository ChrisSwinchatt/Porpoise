#pragma once

#include <porpoise/io/logging.hpp>

#define PORPOISE_ABORT(...)                                    \
    do                                                         \
    {                                                          \
        {                                                      \
            auto __log = ::porpoise::io::logging::log::error();\
            __log << __VA_ARGS__;                              \
        }                                                      \
        ::porpoise::abort();                                   \
    } while (0)

#define PORPOISE_UNREACHABLE(...)       \
    PORPOISE_ABORT(                     \
        "At " __FILE__ ":"              \
        << __LINE__                     \
        << ": in "                      \
        << __func__                     \
        << " BUG: hit unreachable code "\
        << __VA_ARGS__                  \
    )

namespace porpoise
{
    extern "C" void abort() __attribute__((noreturn));
}
