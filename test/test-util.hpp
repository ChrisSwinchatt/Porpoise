#pragma once

#include <cstdlib>
#include <limits>

#include <memory/basic-allocator.hpp>

template <class T>
T random(const T& min = T(), const T& max = std::numeric_limits<T>::max())
{
    auto range = max - min;
    return min + static_cast<T>(std::rand()%max);
}

template <class T>
struct test_allocator : basic_allocator<T>
{
    using size_type  = typename basic_allocator<T>::size_type;
    using value_type = typename basic_allocator<T>::value_type;
    using pointer    = typename basic_allocator<T>::pointer;

    pointer allocate() override
    {
        return reinterpret_cast<pointer>(malloc(sizeof(T)));
    }

    pointer allocate(size_type count) override
    {
        return reinterpret_cast<pointer>(malloc(count*sizeof(T)));
    }
    
    pointer reallocate(pointer p, size_type count) override
    {
        return reinterpret_cast<pointer>(realloc(p, count));
    }

    void deallocate(pointer p) override
    {
        free(p);
    }
};
