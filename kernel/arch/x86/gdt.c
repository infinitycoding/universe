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

#include <gdt.h>
#include <tss.h>

static struct gdt_entry GDT[7]; //nulldesc,Datadesc0,Codedesc0,Datadesc3,Codedesc3,TSS,Callgate 6 descs +1 opt.
static struct gdtpt gdtp;
tss_s tss = { .ss0 = 0x10 };

/**
	@brief sets gdt entry
	@param entry 	GDT Index
	@param base 	Linear segment base address
	@param size 	Segment size (Limit)
	@param access 	Access Settings
	@param flags	Flags
 */
void set_GDT_entry(int entry, uint32_t base, uint32_t size, uint8_t access, int8_t flags)
{
    GDT[entry].limit_low = (uint16_t)size;
    GDT[entry].Base_low = (uint16_t)base;
    GDT[entry].Base_middle = (uint8_t)(base >> 16);
    GDT[entry].Access = access;
    GDT[entry].limit_Flags= (uint8_t)((flags << 4) | (size >> 16));
    GDT[entry].base_high = (uint8_t)(base >> 24);
}

/**
	@brief Load GDT into register
	@param last_entry last entry
 */
void load_gdt(uint16_t last_entry)
{
    gdtp.limit = ((last_entry + 1) * 8) - 1;
    gdtp.base = GDT;
    asm volatile("lgdt %0"::"m" (gdtp));
}

/**
 * @brief set the kernel stack for syscalls
 * @param adress of the kernel stack
 */
void set_kernelstack(void *stack)
{
    tss.esp0 = (uint32_t)stack;
}


void INIT_GDT(void)
{
    set_GDT_entry(0,0,0,0,0);
    //Ring 0 Descriptors
    set_GDT_entry(1,0,0xFFFFF,0x9A,0xC);
    set_GDT_entry(2,0,0xFFFFF,0x92,0xC);

    //Ring 3 Descriptors
    set_GDT_entry(3,0,0xFFFFF,0xFA,0xC);
    set_GDT_entry(4,0,0xFFFFF,0xF2,0xC);
    set_GDT_entry(5, (uint32_t) &tss, sizeof(tss), 0x89, 0x8); //qemu does not support TSS-Desc on position 7... wiered hardware stuff
    load_gdt(5);

    asm volatile(
        "mov $0x10, %ax;"
        "mov %ax, %ds;"
        "mov %ax, %es;"
        "mov %ax, %fs;"
        "mov %ax, %gs;"
        "mov %ax, %ss;"
        "ljmp $0x8, $.1;"
        ".1:;"
    );

    asm volatile("ltr %%ax" : : "a" (5 << 3));
}
