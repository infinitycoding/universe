ENTRY (start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
  . = 0x00100000;
	kernel_start = .;
  .text :
  {
    *(.text)
  }
	
  .rodata :
  {
    *(.rodata)
  }

  .data : 
  {
    *(.data)
  }

  .bss :
  {
    _sbss = .;
    *(COMMON)
    *(.bss)
    _ebss = .;
  }
      . = ALIGN(4096);
    kernel_end = .;
}