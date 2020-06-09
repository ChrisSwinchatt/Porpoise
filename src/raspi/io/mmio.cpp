#include <stdint.h>

#include <porpoise/io/mmio.hpp>

namespace porpoise { namespace io {
    void mmio::put(reg r, uint32_t value)
    {
        *reinterpret_cast<volatile uint32_t*>(r) = value;
    }

    uint32_t mmio::get(reg r)
    {
        return *reinterpret_cast<volatile uint32_t*>(r);
    }
}} // porpose::io
