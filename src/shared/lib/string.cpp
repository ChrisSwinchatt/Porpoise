#include <string.hpp>

size_t strlen(const char* s)
{
    auto p = s;
    while (*s)
    {
        s++;
    }

    return static_cast<size_t>(s - p);
}
