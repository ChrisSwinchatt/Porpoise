ENTRY(_start)
SECTIONS
{
    . = 0x80000;
    __kernel_start__ = .;
    .text BLOCK(4K) : ALIGN(4K) {
        __code_start__ = .;
        KEEP(*(.text.boot))
		*(.text*)
		*(.gnu.linkonce.t*)
        *(.init)
        *(.fini)
        __code_end__   = .;
    }
    .data BLOCK(4K) : ALIGN(4K) {
        __data_start__ = .;
		*(.data)
        *(.gnu.linkonce.d*)
        *(.jcr)
        __data_end__ = .;
    }
    .rodata BLOCK(4K) : ALIGN(4K) {
        __rodata_start__ = .;
        *(.rodata*)
        *(.gnu.linkonce.r*)
        *(.init_array)
        *(.ctors*)
        *(.fini_array)
        *(.dtors*)
        __rodata_end__ = .;
    }
    .eh_frame BLOCK(4K) : ALIGN(4K) {
        start_eh_frame = .;
        __eh_frame_start__ = .;
        *(.eh_frame)
        *(.gnu.linkonce.e*)
        QUAD(0)
        __eh_frame_end__ = .;
    }
    .bss BLOCK(4K) : ALIGN(4K) {
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
