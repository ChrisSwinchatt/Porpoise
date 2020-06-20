#include <algorithm.hpp>
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

void* memcpy(void* dst, const void* src, size_t n)
{
    copy_forwards(reinterpret_cast<const uint8_t*>(src), reinterpret_cast<const uint8_t*>(src) + n, reinterpret_cast<uint8_t*>(dst));
    return dst;
}

void* memmove(void* dst, const void* src, size_t n)
{
    safe_copy(reinterpret_cast<const uint8_t*>(src), reinterpret_cast<const uint8_t*>(src) + n, reinterpret_cast<uint8_t*>(dst));
    return dst;
}
