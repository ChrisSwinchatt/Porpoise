ENTRY(_start)
SECTIONS
{
    . = 0x80000;
    __kernel_start__ = .;
    .text : ALIGN(16) {
        __code_start__ = .;
        KEEP(*(.text.boot))
        *(.fini)
		*(.text*)
		*(.gnu.linkonce.t*)
        __code_end__   = .;
    }
    .init_fini : ALIGN(16) {
        *(.init)
        __init_array_start__ = .;
        *(.init_array)
        __init_array_end__   = .;
        *(.fini)
        __fini_array_start__ = .;
        *(.fini_array)
        __fini_array_end__   = .;
    }
    .data : ALIGN(16) {
        __data_start__ = .;
		*(.data)
		*(.data.*)
        *(.gnu.linkonce.d*)
        *(.jcr)
        __data_end__ = .;
    }
    .rodata : ALIGN(16) {
        __rodata_start__ = .;
        *(.rodata*)
        *(.gnu.linkonce.r*)
        __rodata_end__ = .;
    }
    .eh_frame : ALIGN(16) {
        start_eh_frame = .;
        __eh_frame_start__ = .;
        *(.eh_frame)
        *(.gnu.linkonce.e*)
        QUAD(0)
        __eh_frame_end__ = .;
    }
    .bss : ALIGN(16) {
        __bss_start__ = .;
        *(.bss)
        *(.gnu.linkonce.b*)
        *(COMMON)
        *(.common)
        *(.gnu.linkonce.c*)
        __bss_end__ = .;
	}
    __kernel_end__ = .;
    /DISCARD/ : {
        *(.comment)
    }
}
