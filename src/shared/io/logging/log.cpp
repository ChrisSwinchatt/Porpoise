#include <limits.h>
#include <stdint.h>

#include <porpoise/io/logging.hpp>
#include <porpoise/time/timespan.hpp>

namespace porpoise { namespace io { namespace logging {
    using time::timespan;

    log_level      log::_minimum_level;
    log_sink*      log::_sinks[MAX_SINK];
    int            log::_num_sinks;
    sync::spinlock log::_lock;

    log log::get(log_level level, const char* lvlstr)
    {
        auto inst   = log(level);
        auto millis = timespan::get_program_counter().millis();
        auto secs   = millis/1000;
        millis -= secs*1000;
        inst.emit(secs);
        inst.emit('.');
        inst.emit(millis);
        inst.emit(' ');
        inst.emit(lvlstr);
        inst.emit("] ");
        return inst;
    }

    log log::trace()
    {
        return get(log_level::trace, "trac");
    }

    log log::debug()
    {
        return get(log_level::debug, "debu");
    }

    log log::info()
    {
        return get(log_level::info, "info");
    }

    log log::warn()
    {
        return get(log_level::warn, "warn");
    }

    log log::error()
    {
        return get(log_level::error, "err]");
    }

    bool log::add_sink(log_sink* sink)
    {
        if (sink == nullptr)
        {
            return false;
        }

        if (_num_sinks >= MAX_SINK)
        {
            return false;
        }

        _sinks[_num_sinks++] = sink;
        return true;
    }

    void log::minimum_level(log_level next)
    {
        _minimum_level = next;
    }

    log_level log::minimum_level()
    {
        return _minimum_level;
    }

    int log::num_sinks()
    {
        return _num_sinks;
    }

    log::log(log_level level)
    : _current_level(level)
    , _field_width(0)
    , _fill_char(' ')
    , _base(10)
    , _prefix(false)
    {
        _lock.acquire();
    }

    log::~log()
    {
        emit("\r\n");
        _lock.release();
    }

    void log::emit(char c)
    {
        if (_current_level < _minimum_level)
        {
            return;
        }

        for (auto i = 0; i < _num_sinks; i++)
        {
            _sinks[i]->emit(_current_level, c);
        }
    }

    void log::emit(const char* string)
    {
        if (_current_level < _minimum_level)
        {
            return;
        }

        for (auto i = 0; i < _num_sinks; i++)
        {
            _sinks[i]->emit(_current_level, string);
        }
    }

    void log::emit(intmax_t number)
    {
        if (_current_level < _minimum_level)
        {
            return;
        }

        if (number < 0)
        {
            emit('-');
            emit(static_cast<uintmax_t>(-number));
        }
        else
        {
            emit(static_cast<uintmax_t>(number));
        }
    }

    void log::emit(uintmax_t number)
    {
        if (_current_level < _minimum_level)
        {
            return;
        }

        static constexpr unsigned DIGIT_MAX = sizeof(uintmax_t)*CHAR_BIT;
        static char buffer[DIGIT_MAX];
        auto p     = buffer;
        auto q     = buffer + DIGIT_MAX;
        auto alpha = _hexupper ? 'A' : 'a';
        if (number == 0)
        {
            *p++ = '0';
        }
        else
        {
            do
            {
                auto r = number%_base;
                if (r < 10)
                {
                    *p++ = '0' + r;
                }
                else
                {
                    *p++ = alpha + r - 10;
                }
            } while (number /= _base && p < q);
        }

        auto count = p - buffer;
        while (count > _field_width)
        {
            emit(_fill_char);
        }

        while (p >= buffer)
        {
            emit(*p--);
        }
    }

    uint8_t log::field_width() const
    {
        return _field_width;
    }

    char log::fill_char() const
    {
        return _fill_char;
    }

    uint8_t log::base() const
    {
        return _base;
    }

    bool log::prefix() const
    {
        return _prefix;
    }

    bool log::boolalpha() const
    {
        return _boolalpha;
    }

    bool log::hexupper() const
    {
        return _hexupper;
    }

    void log::field_width(uint8_t next)
    {
        _field_width = next;
    }

    void log::fill_char(char next)
    {
        _fill_char = next;
    }

    void log::base(uint8_t next)
    {
        if (next < 2)
        {
            return;
        }

        _base = next;
    }

    void log::prefix(bool next)
    {
        _prefix = next;
    }

    void log::boolalpha(bool next)
    {
        _boolalpha = next;
    }

    void log::hexupper(bool next)
    {
        _hexupper = next;
    }
}}} // porpoise::io::logging
