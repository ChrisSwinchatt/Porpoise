#pragma once

#include <stddef.h>

// Interface implemented by an allocator.
template <class T>
struct basic_allocator {
    using size_type  = size_t;
    using value_type = T;
    using pointer    = T*;

    /// Allocate a single object of type T.
    virtual pointer allocate() = 0;

    /// Allocate a buffer that can store 'count' objects of T.
    virtual pointer allocate(size_type count) = 0;
    
    /// Reallocate a buffer either upwards or downwards so it can store 'count' objects.
    /// \return nullptr if the buffer could not be resized; p if the buffer was resized in place (i.e. no copying is
    /// required); otherwise, a new pointer to which the contents of p should be copied.
    virtual pointer reallocate(pointer p, size_type count) = 0;

    /// Free the buffer pointed by p. p does not need to point to the start of the buffer. Aborts on double-free or if p
    /// is null.
    virtual void deallocate(pointer p) = 0;
};
