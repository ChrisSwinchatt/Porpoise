#pragma once

#include <porpoise/boot-platform.hpp>

#ifndef __ASM_SOURCE__
#include <stdint.h>

#include <porpoise/capability.hpp>

namespace porpoise { namespace boot {
    /// Run the function fn on the CPU with ID id.
    void start_cpu(int id, void(* fn)());

    /// Post-boot entry point for the CPU with ID id.
    void cpu_main(int id, const heap_capability& heap_cap);
}} // porpoise::boot

#endif // ! __ASM_SOURCE__
