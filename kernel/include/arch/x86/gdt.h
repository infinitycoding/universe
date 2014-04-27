#ifndef	_GDT_H_
#define	_GDT_H_

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

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdint.h>


struct gdt_entry {
	uint16_t limit_low;
	uint16_t Base_low;
	uint8_t Base_middle;
	uint8_t Access;
	uint8_t limit_Flags;
	uint8_t base_high;
};

struct gdtpt{
	uint16_t limit;
	void* base;
} __attribute__((packed));

void set_GDT_entry(int entry, uint32_t base, uint32_t size, uint8_t acess, int8_t flags);
void load_gdt(uint16_t last_entry);
void INIT_GDT(void);

#endif
