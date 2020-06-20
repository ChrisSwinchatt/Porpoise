#include <porpoise/boot.hpp>

.section ".text.boot"

.equ BOOT_ORIGIN, PORPOISE_BOOT_ORIGIN
.equ NUM_CPU,     4
.equ STACK_SIZE,  BOOT_ORIGIN/NUM_CPU

.globl _start

.org BOOT_ORIGIN

_start:
    // Get the CPU ID from the MPIDR 
    mrs     x1,     mpidr_el1
    and     x1,     x1,     #3

    // For CPU 0, SP grows downwards from BOOT_ORIGIN.
    mov     sp,     #BOOT_ORIGIN
    sub     sp,     sp,     #4
    cbz     x1,     boot_kernel     // x0=DTB and x1=0.
    
#if CONFIG_SMP_ENABLED
    // For CPU 1-3, SP grows downwards from `BOOT_ORIGIN - STACK_SIZE*cpu`. This gives each CPU 8K up to Pi 2.
    mov     x3,     #STACK_SIZE
    mul     x3,     x3,     x1      // x3 = STACK_SIZE*cpu
    sub     sp,     sp,     x3      // sp = BOOT_ORIGIN - STACK_SIZE*cpu
    b       boot_kernel             // x0=0 and x1=CPU ID
#endif

_hang:
    b       _hang
