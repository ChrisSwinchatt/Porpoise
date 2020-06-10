#pragma once

#include <limits.h>
#include <stdint.h>

#include <porpoise/io/logging/log.hpp>

namespace porpoise { namespace io { namespace logging {
    struct reset : manipulator
    {
        void operator()(log& log_)
        {
            log_.base(10);
            log_.prefix(false);
            log_.hexupper(false);
            log_.boolalpha(false);
            log_.field_width(0);
            log_.fill_char(' ');
        }
    };

    struct set_width : manipulator
    {
        set_width(uint8_t value) : _value(value) {}
        void operator()(log& log_) {log_.field_width(_value);}
    private:
        uint8_t _value;
    };

    struct set_fill : manipulator
    {
        set_fill(char value) : _value(value) {}
        void operator()(log& log_) {log_.fill_char(_value);}
    private:
        char _value;
    };

    struct set_base : manipulator
    {
        set_base(uint8_t value) : _value(value) {}
        void operator()(log& log_) {log_.base(_value);}
    private:
        uint8_t _value;
    };

    struct show_prefix : manipulator
    {
        show_prefix(bool value) : _value(value) {}
        void operator()(log& log_) {log_.prefix(_value);}
    private:
        bool _value;
    };

    struct boolalpha : manipulator
    {
        boolalpha(bool value) : _value(value) {}
        void operator()(log& log_) {log_.boolalpha(_value);}
    private:
        bool _value;
    };

    struct hexupper : manipulator
    {
        hexupper(bool value) : _value(value) {}
        void operator()(log& log_) {log_.hexupper(_value);}
    private:
        bool _value;
    };
}}} // porpoise::io::logging

porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, void*);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, char c);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, const char* s);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, int8_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, int16_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, int32_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, int64_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, uint8_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, uint16_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, uint32_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, uint64_t number);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::reset manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::set_width manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::set_fill manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::set_base manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::show_prefix manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::boolalpha manip);
porpoise::io::logging::log& operator<<(porpoise::io::logging::log& log_, porpoise::io::logging::hexupper manip);
