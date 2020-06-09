#include <porpoise/io/logging/manipulators.hpp>

using namespace porpoise::io::logging;

log operator<<(log log_, char c)
{
    log_.emit(c);
    return log_;
}

log operator<<(log log_, const char* s)
{
    log_.emit(s);
    return log_;
}

log operator<<(log log_, int8_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log operator<<(log log_, int16_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log operator<<(log log_, int32_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log operator<<(log log_, int64_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log operator<<(log log_, uint8_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log operator<<(log log_, uint16_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log operator<<(log log_, uint32_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log operator<<(log log_, uint64_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log operator<<(log log_, reset manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, set_width manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, set_fill manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, set_base manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, show_prefix manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, boolalpha manip)
{
    manip(log_);
    return log_;
}

log operator<<(log log_, hexupper manip)
{
    manip(log_);
    return log_;
}
