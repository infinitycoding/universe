#ifndef	_multiboot_h_
#define	_multiboot_h_

/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

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

struct mmap_entry {
	uint32_t size;
	uint64_t BaseAddr;
	uint64_t Length;
	uint32_t Type;
};

struct mods_add {
	uint32_t mod_start;
	uint32_t mod_end;
	char *string;
	uint32_t reserviert;
};

#endif
