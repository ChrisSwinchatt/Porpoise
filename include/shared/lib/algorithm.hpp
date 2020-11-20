#pragma once

#include <algorithm/algorithm-copy.hpp>
#include <algorithm/algorithm-fill.hpp>
#include <algorithm/algorithm-search.hpp>

template <class T>
void swap(T& x, T& y)
{
    T z = x;
    x = y;
    y = z; 
}
