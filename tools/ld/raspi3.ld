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
        __code_end__   = .;
    }
    .init_array :
    {
        crti.o(.init_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .init_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .init_array))
        crtn.o(.init_array)
    }
    .fini_array :
    {
        crti.o(.fini_array)
        KEEP (*(SORT(EXCLUDE_FILE(crti.o crtn.o) .fini_array.*)))
        KEEP (*(EXCLUDE_FILE(crti.o crtn.o) .fini_array))
        crtn.o(.fini_array)
    }
    .data BLOCK(4K) : ALIGN(4K) {
        __data_start__ = .;
		*(.data)
        *(.ctors*)
        *(.dtors*)
        *(.gnu.linkonce.d*)
        *(.jcr)
        __data_end__ = .;
    }
    .rodata BLOCK(4K) : ALIGN(4K) {
        __rodata_start__ = .;
        *(.rodata*)
        *(.gnu.linkonce.r*)
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
