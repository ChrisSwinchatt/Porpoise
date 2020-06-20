#pragma once

#include <string.hpp>

/// Copy forwards from start:end to dest and return the end of dest.
template <class T>
T* copy_forwards(const T* start, const T* end, T* dest)
{
    while (start < end) {
        *dest++ = *start++;
    }
    return dest;
}

/// Copy backwards from start:end to dest and return the end of dest.
template <class T>
T* copy_backwards(const T* start, const T* end, T* dest)
{
    end--;
    dest += end - start;
    while (end >= start) {
        *dest-- = *end--;
    }
    return dest;
}

/// Copy forwards or backwards depending on whether start:end and dest overlap, and return the end of dest.
template <class T>
T* safe_copy(const T* start, const T* end, T* dest)
{
    if (start == dest) {
        return dest;
    } else if (dest < start || dest > end) {
        return copy_forwards(start, end, dest);
    } else {
        return copy_backwards(start, end, dest);
    }
}
