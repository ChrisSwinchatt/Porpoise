#pragma once

#define BOOT_ORIGIN_PI_3_4   0x80000
#define BOOT_ORIGIN_PI_0_1_2 0x8000

#if __raspi__ >= 3
# define PORPOISE_BOOT_ORIGIN BOOT_ORIGIN_PI_3_4
#elif defined(__raspi__)
# define PORPOISE_BOOT_ORIGIN BOOT_ORIGIN_PI_0_1_2
#else
# error Building Raspberry Pi code but __raspi__ is not defined
#endif

#ifndef __ASM_SOURCE__
#include <stdint.h>

namespace porpoise { namespace boot {
    /// Boot the kernel. Can be called from any CPU.
    /// \param dtb Points to the Device Tree Blob. Only the low 32-bits are used. Only used if id==0.
    /// \param id The ID of the executing CPU.
    extern "C" void boot_kernel(uint64_t dtb, uint64_t id) __attribute__((noreturn));
}} // porpoise::boot

#endif // ! __ASM_SOURCE__
