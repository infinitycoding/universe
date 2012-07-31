



ENTRY (start)

SECTIONS
{
 . = 0xC0100000 - 0xC0000000 + SIZEOF_HEADERS;

    .boot ALIGN(4096) : {
        *(.multiboot .multiboot.*)
    }

 _MEMORY_LAYOUT_KERNEL_PSTART = ALIGN(4096);

    .text _MEMORY_LAYOUT_KERNEL_PSTART + 0xC0000000 : AT (_MEMORY_LAYOUT_KERNEL_PSTART) {
        kernel_start = .;
        *(.text .text.*)
    }

    .rodata ALIGN(4096) : {
        *(.rodata .rodata.*)
    }

    .data ALIGN(4096) : {
        *(.data .data.*)
    }

    .bss ALIGN(4096) : {
        *(.bss .bss.*)
    }

    . = ALIGN(4096);
    kernel_end = .;
}
