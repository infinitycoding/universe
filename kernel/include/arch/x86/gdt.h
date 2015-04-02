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
#include <arch_context.h>

typedef enum
{
    NULL_DESC   = 0,
    GATE_DESC   = 0,
    TSS_DESC    = 9,
    DATA_DESC   = 16,
    CODE_DESC   = 24
} gdt_seg_type;

typedef enum
{
    AVAILABLE   = 16,
    LONGMODE    = 32,
    BITS32      = 64,
    GRAN4K      = 128,

    ACCESSED    = 256,
    WRITEABLE   = 512,
    READABLE    = 512,
    DIRECTION   = 1024,
    CONFORMING  = 1024,
    PRESENT     = 32768
} gdt_flag_type;


struct gdt_entry
{
    uint16_t limit_low;
    uint16_t Base_low;
    uint8_t Base_middle;
    uint8_t Access;
    uint8_t limit_Flags;
    uint8_t base_high;
};

struct gdtpt
{
    uint16_t limit;
    void* base;
} __attribute__((packed));

void INIT_GDT();
void gdt_set_entry(struct gdt_entry *gdt, uint16_t entry, paddr_t base, size_t size, privilege_t prev,gdt_seg_type type, gdt_flag_type flags);;
void gdt_load(struct gdt_entry *gdt, uint16_t last_entry);

void set_kernelstack(void *stack);
void set_iobmp(struct arch_thread_context *context);

#endif

