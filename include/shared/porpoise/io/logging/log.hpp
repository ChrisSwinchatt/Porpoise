#pragma once

#include <stdint.h>
#include <stddef.h>

#include <porpoise/sync/semaphore.hpp>

#define PORPOISE_LOG_TRACE(...) do {auto __log = ::porpoise::io::logging::log::trace(); __log << __VA_ARGS__;} while (0)
#define PORPOISE_LOG_DEBUG(...) do {auto __log = ::porpoise::io::logging::log::debug(); __log << __VA_ARGS__;} while (0)
#define PORPOISE_LOG_INFO(...)  do {auto __log = ::porpoise::io::logging::log::info();  __log << __VA_ARGS__;} while (0)
#define PORPOISE_LOG_WARN(...)  do {auto __log = ::porpoise::io::logging::log::warn();  __log << __VA_ARGS__;} while (0)
#define PORPOISE_LOG_ERROR(...) do {auto __log = ::porpoise::io::logging::log::error(); __log << __VA_ARGS__;} while (0)

namespace porpoise { namespace io { namespace logging {
    /// Log level.
    enum class log_level : uint8_t
    {
        trace,
        debug,
        info,
        warn,
        error
    };

    /// Log sink, e.g. serial device, a file or the screen.
    struct log_sink
    {
        /// Emit a string verbatim if the log level is greater than or equal to the sink's log-level.
        virtual void emit(log_level level, const char* event) = 0;

        /// Emit a character verbatim if the log level is greater than or equal to the sink's log-level.
        virtual void emit(log_level level, char c) = 0;
    };

    struct log;

    struct manipulator
    {
        virtual void operator()(log& log_) = 0;
    };

    struct log
    {
        static constexpr int MAX_SINK = 16;

        static log trace();
        
        static log debug();
        
        static log info();
        
        static log warn();
        
        static log error();

        static log_level minimum_level();

        static void minimum_level(log_level next);

        static int num_sinks();

        static bool add_sink(log_sink* sink);

        log(log&& other);

        void operator=(log&& other);

        virtual ~log();

        void emit(char c);

        void emit(const char* string);

        void emit(bool value);

        void emit(intmax_t number);

        void emit(uintmax_t number);

        void flush();

        uint8_t field_width() const;

        char fill_char() const;

        uint8_t base() const;

        bool prefix() const;

        bool boolalpha() const;

        bool hexupper() const;

        void field_width(uint8_t next);

        void fill_char(char next);

        void base(uint8_t next);

        void prefix(bool next);

        void boolalpha(bool next);

        void hexupper(bool next);

        log(const log&) = delete;

        void operator=(const log&) = delete;
    protected:
        explicit log(log_level level);

    private:
        static log_level      _minimum_level;
        static log_sink*      _sinks[MAX_SINK];
        static int            _num_sinks;
        static sync::spinlock _sink_lock;

        static log get(log_level level, const char* lvlstr);

        struct log_internal_state
        {
            log_level current_level;
            uint8_t   field_width;
            char      fill_char;
            uint8_t   base;
            bool      prefix;
            bool      boolalpha;
            bool      hexupper;

            explicit log_internal_state(log_level level)
            : current_level(level)
            , field_width(0)
            , fill_char(' ')
            , base(10)
            , prefix(false)
            , boolalpha(false)
            , hexupper(false)
            {
            }
        } * _state;

        bool is_active_instance() const;

        void internal_emit(char c);

        void internal_emit(const char* s);
    };

}}} // porpoise::io::logging
