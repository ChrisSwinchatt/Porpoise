#include <porpoise/boot.hpp>

.section ".text.boot"

.globl _start

.equ STACK_SIZE, 0x2000

.org BOOT_ORIGIN

_start:
    // Get the CPU ID from the MPIDR 
    mrs     x1,     mpidr_el1
    and     x1,     x1,     #3


    mov     x2,     BOOT_ORIGIN

    // For CPU 0, boot immediately with sp=BOOT_ORIGIN, x0=DTB and x1=0.
0:  cbnz    x1,     N
    mov     sp,     x2
    b       boot_kernel
    
    // For CPU 1-3, set stack pointer to `BOOT_ORIGIN - STACK_SIZE*cpu` giving each CPU an 8 kiB stack.
N:  mov     x3,     STACK_SIZE
    mul     x3,     x3,     x1      // x3 = STACK_SIZE*cpu
    sub     x2,     x2,     x3      // x2 = BOOT_ORIGIN - STACK_SIZE*cpu
    mov     sp,     x2
    b       boot_kernel

_hang:
    b       _hang
