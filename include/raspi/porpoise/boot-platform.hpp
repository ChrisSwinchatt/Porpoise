#pragma once

#if __raspi__ >= 3
# define BOOT_ORIGIN 0x80000
#else
# define BOOT_ORIGIN 0x8000
#endif

#ifndef __ASM_SOURCE__
#include <stdint.h>

namespace porpoise { namespace boot {
    extern "C" void boot_kernel(uint64_t dtb);
}} // porpoise::boot

#endif // ! __ASM_SOURCE__
