#pragma once


#include <porpoise/boot-platform.hpp>

#ifndef __ASM_SOURCE__
#include <stdint.h>

namespace porpoise { namespace boot {
    void start_cpu(int id, void(* fn)());

    void cpu_main(int id);
}} // porpoise::boot

#endif // ! __ASM_SOURCE__
