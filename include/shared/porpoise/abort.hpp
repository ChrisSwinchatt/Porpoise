#pragma once

#include <porpoise/io/logging.hpp>

#define PORPOISE_ABORT(...)                            \
do                                                     \
{                                                      \
    auto __log = ::porpoise::io::logging::log::error();\
    __log << __VA_ARGS__;                              \
    ::porpoise::abort();                               \
} while (0)

namespace porpoise
{
    extern "C" void abort() __attribute__((noreturn));
}
