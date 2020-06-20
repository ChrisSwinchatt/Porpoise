#include <porpoise.hpp>
#include <porpoise/io/logging/manipulators.hpp>

using namespace porpoise::io::logging;

log& operator<<(log& log_, char c)
{
    log_.emit(c);
    return log_;
}

log& operator<<(log& log_, const char* s)
{
    log_.emit(s);
    return log_;
}

log& operator<<(log& log_, int8_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log& operator<<(log& log_, int16_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log& operator<<(log& log_, int32_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log& operator<<(log& log_, int64_t number)
{
    log_.emit(static_cast<intmax_t>(number));
    return log_;
}

log& operator<<(log& log_, uint8_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log& operator<<(log& log_, uint16_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log& operator<<(log& log_, uint32_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log& operator<<(log& log_, uint64_t number)
{
    log_.emit(static_cast<uintmax_t>(number));
    return log_;
}

log& operator<<(log& log_, reset manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, set_width manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, set_fill manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, set_base manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, show_prefix manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, boolalpha manip)
{
    manip(log_);
    return log_;
}

log& operator<<(log& log_, hexupper manip)
{
    manip(log_);
    return log_;
}

porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, nullptr_t p)
{
    PORPOISE_UNUSED(p);
    log_ << "(null)";
    return log_;
}

log& operator<<(log& log_, void* p)
{
    auto old_field_width = log_.field_width();
    auto old_fill_char   = log_.fill_char();
    auto old_base        = log_.base();
    auto old_prefix      = log_.prefix();
    auto old_boolalpha   = log_.boolalpha();
    auto old_hexupper    = log_.hexupper();
    log_ << set_width(2*sizeof(p))
         << set_fill('0')
         << hexupper(true)
         << set_base(16)
         << show_prefix(true)
         << reinterpret_cast<uintptr_t>(p)
    ;
    log_.field_width(old_field_width);
    log_.fill_char(old_fill_char);
    log_.base(old_base);
    log_.prefix(old_prefix);
    log_.boolalpha(old_boolalpha);
    log_.hexupper(old_hexupper);
    return log_;
}
