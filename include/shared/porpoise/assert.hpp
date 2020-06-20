#pragma once

#include <porpoise/macro.hpp>

#include <porpoise/abort.hpp>

#define PORPOISE_CHECK(X)                     __PORPOISE_CHECK_EXPR("check",            X)
#define PORPOISE_CHECK_LESS(A,             B) __PORPOISE_CHECK_VALUES(less,             "check", A, B, #A "<" #B)
#define PORPOISE_CHECK_LESS_OR_EQUAL(A,    B) __PORPOISE_CHECK_VALUES(less_or_equal,    "check", A, B, #A "<=" #B)
#define PORPOISE_CHECK_EQUAL(A,            B) __PORPOISE_CHECK_VALUES(equal,            "check", A, B, #A "==" #B)
#define PORPOISE_CHECK_UNEQUAL(A,          B) __PORPOISE_CHECK_VALUES(unequal,          "check", A, B, #A "!=" #B)
#define PORPOISE_CHECK_GREATER_OR_EQUAL(A, B) __PORPOISE_CHECK_VALUES(greater_or_equal, "check", A, B, #A ">=" #B)
#define PORPOISE_CHECK_GREATER(A,          B) __PORPOISE_CHECK_VALUES(greater,          "check", A, B, #A ">" #B)

#ifdef NDEBUG
# define PORPOISE_ASSERT(X)                         do { PORPOISE_UNUSED(X); } while (0)
# define PORPOISE_ASSERT_LESS(A,             B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
# define PORPOISE_ASSERT_LESS_OR_EQUAL(A,    B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
# define PORPOISE_ASSERT_EQUAL(A,            B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
# define PORPOISE_ASSERT_UNEQUAL(A,          B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
# define PORPOISE_ASSERT_GREATER_OR_EQual(A, B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
# define PORPOISE_ASSERT_GREATER(A,          B)     do { PORPOISE_UNUSED(A); PORPOISE_UNUSED(B); } while (0)
#else
# define PORPOISE_ASSERT(X)                         __PORPOISE_CHECK_EXPR("assert",           X)
# define PORPOISE_ASSERT_LESS(A,             B)     __PORPOISE_CHECK_VALUES(less,             "assert", A, B, #A "<" #B)
# define PORPOISE_ASSERT_LESS_OR_EQUAL(A,    B)     __PORPOISE_CHECK_VALUES(less_or_equal,    "assert", A, B, #A "<=" #B)
# define PORPOISE_ASSERT_EQUAL(A,            B)     __PORPOISE_CHECK_VALUES(equal,            "assert", A, B, #A "==" #B)
# define PORPOISE_ASSERT_UNEQUAL(A,          B)     __PORPOISE_CHECK_VALUES(unequal,          "assert", A, B, #A "!=" #B)
# define PORPOISE_ASSERT_GREATER_OR_EQUAL(A, B)     __PORPOISE_CHECK_VALUES(greater_or_equal, "assert", A, B, #A ">=" #B)
# define PORPOISE_ASSERT_GREATER(A,          B)     __PORPOISE_CHECK_VALUES(greater,          "assert", A, B, #A ">" #B)
#endif

#define __PORPOISE_CHECK_EXPR(ACTION, X)                 ::porpoise::check::expr(__FILE__, __LINE__, __func__, ACTION, (X), #X)

#define __PORPOISE_CHECK_VALUES(FUNC, ACTION, A, B, STR) ::porpoise::check::FUNC(__FILE__, __LINE__, __func__, ACTION, (A), (B), STR)

#define __PORPOISE_CHECK_FAIL(FILE, LINE, FUNC, ACTION, A, OP, B, EXPR_STR)\
    PORPOISE_ABORT("At " << FILE << ":" << LINE << " in " << FUNC << ": " << ACTION << " failed: `" << EXPR_STR << "` (" << A << OP << B << ")")

namespace porpoise { namespace check {
    void expr(const char* file, int line, const char* func, const char* action, bool result, const char* expr_as_string);

    template <class Ta, class Tb>
    void less(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a >= b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, "<", b, expr_str);
        }
    }

    template <class Ta, class Tb>
    void less_or_equal(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a > b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, "<=", b, expr_str);
        }
    }

    template <class Ta, class Tb>
    void equal(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a != b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, "==", b, expr_str);
        }
    }

    template <class Ta, class Tb>
    void unequal(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a == b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, "!=", b, expr_str);
        }
    }

    template <class Ta, class Tb>
    void greater_or_equal(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a < b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, ">=", b, expr_str);
        }
    }

    template <class Ta, class Tb>
    void greater(const char* file, int line, const char* func, const char* action, const Ta&a, const Tb& b, const char* expr_str)
    {
        if (a <= b)
        {
            __PORPOISE_CHECK_FAIL(file, line, func, action, a, ">", b, expr_str);
        }
    }

    template <class T>
    struct value_incremented_scope {
        explicit value_incremented_scope(T& value, const char* file, int line, const char* func)
        : _ref(value)
        , _file(file)
        , _line(line)
        , _func(func)
        {
            _initial = value;
        }

        virtual ~value_incremented_scope()
        {
            if (_ref != _initial + 1)
            {
                __PORPOISE_CHECK_FAIL(_file, _line, _func, "check", _ref, "==", (_initial + 1), "incremented");
            }
        }
    private:
        T _initial;
        T& _ref;
        const char* _file;
        int _line;
        const char* _func;
    };

    template <class T>
    struct value_decremented_scope {
        explicit value_decremented_scope(T& value, const char* file, int line, const char* func)
        : _ref(value)
        , _file(file)
        , _line(line)
        , _func(func)
        {
            _initial = value;
        }

        virtual ~value_decremented_scope()
        {
            if (_ref != _initial--)
            {
                __PORPOISE_CHECK_FAIL(_file, _line, _func, "check", _ref, "==", _initial, "decremented");
            }
        }
    private:
        T _initial;
        T& _ref;
        const char* _file;
        int _line;
        const char* _func;
    };

    template <class T>
    struct value_changed_scope {
        explicit value_changed_scope(T& value, const char* file, int line, const char* func)
        : _ref(value)
        , _file(file)
        , _line(line)
        , _func(func)
        {
            _initial = value;
        }

        virtual ~value_changed_scope()
        {
            if (_ref == _initial)
            {
                __PORPOISE_CHECK_FAIL(_file, _line, _func, "check", _ref, "!=", _initial, "changed");
            }
        }
    private:
        T _initial;
        T& _ref;
        const char* _file;
        int _line;
        const char* _func;
    };

    template <class T>
    struct value_unchanged_scope {
        explicit value_unchanged_scope(T& value, const char* file, int line, const char* func)
        : _ref(value)
        , _file(file)
        , _line(line)
        , _func(func)
        {
            _initial = value;
        }

        virtual ~value_unchanged_scope()
        {
            if (_ref != _initial)
            {
                __PORPOISE_CHECK_FAIL(_file, _line, _func, "check", _ref, "==", _initial, "unchanged");
            }
        }
    private:
        T _initial;
        T& _ref;
        const char* _file;
        int _line;
        const char* _func;
    };
}}
