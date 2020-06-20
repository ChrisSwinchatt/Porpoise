#include <porpoise/assert.hpp>

namespace porpoise { namespace check {
    void expr(const char* file, int line, const char* func, const char* action, bool result, const char* expr_as_string)
    {
        if (!result)
        {
            PORPOISE_ABORT("At " << file << ":" << line << " in " << func << ": " << action << " failed: " << expr_as_string);
        }
    }
}} // porpoise::check
