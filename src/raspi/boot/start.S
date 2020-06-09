#include <porpoise/boot.hpp>

.section ".text.boot"

.globl _start

.org BOOT_ORIGIN

_start:
    // x0 points to the DTB.
    ldr     x4,     =_start
    mov     sp,     x4
    bl      boot_kernel
_exit:
    wfe
    b       _exit
