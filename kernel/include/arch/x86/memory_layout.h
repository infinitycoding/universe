#ifndef _MEMORY_LAYOUT_H_
#define _MEMORY_LAYOUT_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.
 
     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef LINKER
	extern void kernel_phys_start;
	extern void kernel_phys_end;
	extern void kernel_virt_start;
	extern void kernel_virt_end;
#endif

#define MEMORY_LAYOUT_DIRECT_MAPPED		0x02000000

#define PAGE_SIZE				4096

#define MEMORY_LAYOUT_STACK_TOP			0xC0000000
#define MEMORY_LAYOUT_KERNEL_START		0xC0000000
#define MEMORY_LAYOUT_KERNEL_EXEC_START		0xC0100000
#define MEMORY_LAYOUT_RESERVED_AREA_END		0xC2000000
#define MEMORY_LAYOUT_KERNEL_END		0xFFFFFFFF
#define MEMORY_LAYOUT_KERNEL_HEAP_START		0xE0000000
#define MEMORY_LAYOUT_KERNEL_HEAP_END		0xFFFFFFFF
#define MEMORY_LAYOUT_USER_HEAP_START		0xA0000000
#define MEMORY_LAYOUT_USER_HEAP_END		0xB0000000

#define PAGING_STRUCTURES_COUNT			32
#define MEMORY_LAYOUT_PAGING_STRUCTURES_START	0xFF800000 /* 4 GB - 8 MB */

#endif /* MEMORY_LAYOUT_H_ */
