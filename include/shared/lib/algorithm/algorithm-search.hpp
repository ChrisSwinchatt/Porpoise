#pragma once

#include <comparer.hpp>

#include <porpoise/assert.hpp>

template <class T, class ForwardIterator, class Comparer = default_comparer<T>>
ForwardIterator linear_search(const T& value, ForwardIterator start, ForwardIterator end, const Comparer& comparer = Comparer())
{
    PORPOISE_ASSERT(start <= end);
    if (start == end)
    {
        return ++end;
    }

    for (; start < end; start++)
    {
        if (comparer(*start, value) == 0)
        {
            return start;
        }
    }

    return ++end;
}

template <class T, class ForwardIterator, class Comparer = default_comparer<T>>
ForwardIterator binary_search(const T& value, ForwardIterator start, ForwardIterator end, const Comparer& comparer = Comparer())
{
    PORPOISE_ASSERT(start <= end);
    if (start == end)
    {
        return ++end;
    }

    auto size = end - start;
    if (size == 1)
    {
        if (comparer(start[0], value) == 0)
        {
            return start;
        }

        return ++end;
    }

    auto half = size/2;
    auto comp = comparer(start[half], value);
    if (comp < 0)
    {
        return binary_search(value, start + half, end, comparer);
    }

    if (comp > 0)
    {
        return binary_search(value, start, start + half, comparer);
    }

    return &start[half];
}
