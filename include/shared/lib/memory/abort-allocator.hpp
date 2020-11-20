#pragma once

#include <porpoise/macro.hpp>
#include <memory/basic-allocator.hpp>

#include <porpoise/abort.hpp>

/// An allocator which calls abort if it is used. Pass to containers if you want them to have a fixed size.
template <class T>
struct abort_allocator : basic_allocator<T> {
    using size_type  = size_t;
    using value_type = T;
    using pointer    = T*;

    static abort_allocator<T> instance;

    virtual T* allocate() override
    {
        PORPOISE_ABORT("Abort allocator used to allocate");
        return nullptr;
    }

    virtual T* allocate(size_t count) override
    {
        PORPOISE_UNUSED(count);
        PORPOISE_ABORT("Abort allocator used to allocate");
        return nullptr;
    }
    
    virtual T* reallocate(T* p, size_t count) override
    {
        PORPOISE_UNUSED(p);
        PORPOISE_UNUSED(count);
        PORPOISE_ABORT("Abort allocator used to reallocate");
        return nullptr;
    }

    virtual void deallocate(T* p) override
    {
        PORPOISE_UNUSED(p);
        PORPOISE_ABORT("Abort allocator used to deallocate");
    }
};

template <class T>
abort_allocator<T> abort_allocator<T>::instance;
