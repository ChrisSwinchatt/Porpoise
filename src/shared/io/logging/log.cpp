#include <stdint.h>
#include <stddef.h>

#include <string.hpp>

#include <porpoise.hpp>
#include <porpoise/heap.hpp>
#include <porpoise/io/logging.hpp>
#include <porpoise/time/timespan.hpp>
#include <porpoise/sync/lock-guard.hpp>
#include <porpoise/sync/spinlock.hpp>

namespace porpoise { namespace io { namespace logging {
    using namespace time;
    using namespace sync;

    log_level log::_minimum_level;
    log_sink* log::_sinks[MAX_SINK];
    int       log::_num_sinks;
    spinlock  log::_sink_lock;

    log log::get(log_level level, const char* lvlstr)
    {
        _sink_lock.acquire();
        log inst(level);
        auto millis = timespan::get_program_counter().millis();
        auto secs   = millis/1000;
        millis -= secs*1000;
        inst << secs 
             << '.' 
             << set_width(3) 
             << set_fill('0') 
             << millis 
             << reset() 
             << ' ' 
             << lvlstr 
             << "] "
        ;
        return inst;
    }

    log log::trace()
    {
        return get(log_level::trace, "trace");
    }

    log log::debug()
    {
        return get(log_level::debug, "debug");
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
        return get(log_level::error, "error");
    }

    bool log::add_sink(log_sink* sink)
    {
        lock_guard guard(_sink_lock);
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

    log::log(log_level level) : _state(new log_internal_state(level))
    {
    }

    log::log(log&& other)
    {
        _state       = other._state;
        other._state = nullptr;
    }

    void log::operator=(log&& other)
    {
        _state = other._state;
        other._state = nullptr;
    }

    bool log::is_active_instance() const
    {
        return _state != nullptr;
    }

    log::~log()
    {
        if (is_active_instance())
        {
            internal_emit("\r\n");
            delete _state;
            _state = nullptr;
            _sink_lock.release();
        }
    }

    void log::internal_emit(const char* s)
    {
        for (auto i = 0; i < _num_sinks; i++)
        {
            _sinks[i]->emit(_state->current_level, s);
        }
    }

    void log::internal_emit(char c)
    {
        for (auto i = 0; i < _num_sinks; i++)
        {
            _sinks[i]->emit(_state->current_level, c);
        }
    }

    void log::emit(char c)
    {
        if (!is_active_instance() || _state->current_level < _minimum_level)
        {
            return;
        }

        for (auto j = 1; j < _state->field_width; j++)
        {
            internal_emit(_state->fill_char);
        }

        internal_emit(c);
    }

    void log::emit(const char* string)
    {
        if (!is_active_instance() || _state->current_level < _minimum_level)
        {
            return;
        }

        for (auto j = 1; j < _state->field_width; j++)
        {
            internal_emit(_state->fill_char);
        }

        internal_emit(string);
    }

    void log::emit(intmax_t number)
    {
        if (!is_active_instance() || _state->current_level < _minimum_level)
        {
            return;
        }

        if (number < 0)
        {
            internal_emit('-');
            emit(static_cast<uintmax_t>(-number));
        }
        else
        {
            emit(static_cast<uintmax_t>(number));
        }
    }

    void log::emit(uintmax_t number)
    {
        if (!is_active_instance() || _state->current_level < _minimum_level)
        {
            return;
        }

        static constexpr unsigned DIGIT_MAX = sizeof(uintmax_t)*CHAR_BIT;
        char buffer[DIGIT_MAX];
        auto p     = buffer;
        auto q     = buffer + DIGIT_MAX;
        auto alpha = _state->hexupper ? 'A' : 'a';
        do
        {
            auto r = number%_state->base;
            if (r < 10)
            {
                *p++ = '0' + r;
            }
            else
            {
                *p++ = alpha + r - 10;
            }

            number /= _state->base;
        } while (number && p < q);

        for (auto count = p - buffer; count < _state->field_width; count++)
        {
            internal_emit(_state->fill_char);
        }

        while (p-- > buffer)
        {
            internal_emit(*p);
        }
    }

    uint8_t log::field_width() const
    {
        if (!is_active_instance())
        {
            return 0;
        }

        return _state->field_width;
    }

    char log::fill_char() const
    {
        if (!is_active_instance())
        {
            return 0;
        }

        return _state->fill_char;
    }

    uint8_t log::base() const
    {
        if (!is_active_instance())
        {
            return 0;
        }

        return _state->base;
    }

    bool log::prefix() const
    {
        if (!is_active_instance())
        {
            return false;
        }

        return _state->prefix;
    }

    bool log::boolalpha() const
    {
        if (!is_active_instance())
        {
            return false;
        }

        return _state->boolalpha;
    }

    bool log::hexupper() const
    {
        if (!is_active_instance())
        {
            return false;
        }

        return _state->hexupper;
    }

    void log::field_width(uint8_t next)
    {
        if (!is_active_instance())
        {
            return;
        }

        _state->field_width = next;
    }

    void log::fill_char(char next)
    {
        if (!is_active_instance())
        {
            return;
        }

        _state->fill_char = next;
    }

    void log::base(uint8_t next)
    {
        if (!is_active_instance())
        {
            return;
        }

        if (next < 2)
        {
            return;
        }

        _state->base = next;
    }

    void log::prefix(bool next)
    {
        if (!is_active_instance())
        {
            return;
        }

        _state->prefix = next;
    }

    void log::boolalpha(bool next)
    {
        if (!is_active_instance())
        {
            return;
        }

        _state->boolalpha = next;
    }

    void log::hexupper(bool next)
    {
        if (!is_active_instance())
        {
            return;
        }

        _state->hexupper = next;
    }
}}} // porpoise::io::logging
