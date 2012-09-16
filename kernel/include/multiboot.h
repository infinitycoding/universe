#ifndef	_multiboot_h_
#define	_multiboot_h_
#include <stdint.h>

	struct multiboot_struct {
		uint32_t flags;
		uint32_t mem_low;
		uint32_t mem_up;
		uint32_t bootdevice;
		uint32_t cmdline;
		uint32_t mods_count;
		uint32_t mods_addr;
		uint64_t syms_low; 
		uint64_t syms_high;
		uint32_t mmap_length;
		uint32_t mmap_addr;
		uint32_t drives_length; 
		uint32_t drives_addr;
		uint32_t config_table;
		uint32_t boot_loader_name;
		uint32_t apm_table;
		uint32_t vbe_control_info; 
		uint32_t vbe_mode_info;
		uint16_t vbe_mode;
		uint16_t vbe_interface_seg;
		uint16_t vbe_interface_off;
		uint16_t vbe_interface_len;
	};

	struct mmap_entry{
		uint32_t size;
		uint64_t BaseAddr;
		uint64_t Length;
		uint32_t Type;
	};

	struct mods_add{
		uint32_t mod_start;
		uint32_t mod_end;
		char *string;
		uint32_t reserviert;
	};

#endif
