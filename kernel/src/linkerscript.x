



ENTRY (start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
      .  = 0x00100000;
      kernel_phys_start = .;

      . += 0xC0000000;
      kernel_virt_start = .;

      .text : AT(kernel_phys_start) {
          *(.multiboot*)
          *(.text)
      }

      .data ALIGN(4096) : {
          *(.data)
      }

      .bss ALIGN(4096) : {
	  *(.bss*)
      }

      kernel_virt_end = .;
      kernel_phys_end = . - 0xC0000000;
}
