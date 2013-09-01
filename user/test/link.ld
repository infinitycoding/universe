ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
     . = 0x200000;
     .text : {
         *(.text)
     }
     .data ALIGN(4096) : {
         *(.data)
     }
     .rodata ALIGN(4096) : {
         *(.rodata)
     }
     .bss ALIGN(4096) : {
         *(.bss)
     }
}

