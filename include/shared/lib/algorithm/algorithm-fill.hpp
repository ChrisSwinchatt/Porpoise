#pragma once

#include <stdint.h>

constexpr int UNROLL = 4;

/// Fill the region of memory between `start` and `end` with `value`.
template <class T>
void fill_region(T* start, T* end, const T& value)
{
    if (end - start > UNROLL)
    {
        while (start < end - UNROLL)
        {
            *start++ = value;
            *start++ = value;
            *start++ = value;
            *start++ = value;
        }
    }

    while (start < end)
    {
        *start++ = value;
    }
}

/// Fill the region of memory between `start` and `end` with zero bytes.
template <class T>
void zero_region(T* start, T* end)
{
    fill_region(reinterpret_cast<uint8_t*>(start), reinterpret_cast<uint8_t*>(end), static_cast<uint8_t>(0));
}

/// Fill the contents of `obj` with zero bytes.
template <class T>
void zero_object(T& obj)
{
    zero_region(&obj, &obj + 1);
}
