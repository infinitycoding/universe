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
 *  @file /arch/x86/gdt.c
 *  @brief Modul for x86 memory segment/previleg level system
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <gdt.h>
#include <tss.h>
#include <stddef.h>
#include <string.h>


static struct gdt_entry GDT[6]; //nulldesc,Datadesc0,Codedesc0,Datadesc3,Codedesc3,TSS
tss_s tss = { .ss0 = 0x10 };

/**
 * @brief INIT function for x86 segment/previleg level system
 */
void INIT_PREV(void)
{
    //NULL Descriptor
    gdt_set_entry(GDT, 0, 0, 0, 0, NULL_DESC, 0);

    //Ring 0 Descriptors
    gdt_set_entry(GDT, 1, 0, 0xFFFFF, KERNELMODE, CODE_DESC, BITS32 | GRAN4K | READABLE | PRESENT);
    gdt_set_entry(GDT, 2, 0, 0xFFFFF, KERNELMODE, DATA_DESC, BITS32 | GRAN4K | WRITEABLE | PRESENT);

    //Ring 3 Descriptors
    gdt_set_entry(GDT, 3, 0, 0xFFFFF, USERMODE, CODE_DESC, BITS32 | GRAN4K | READABLE | PRESENT);
    gdt_set_entry(GDT, 4, 0, 0xFFFFF, USERMODE, DATA_DESC, BITS32 | GRAN4K | WRITEABLE | PRESENT);

    //TSS descriptor
    gdt_set_entry(GDT, 5, (paddr_t)&tss, sizeof(tss), KERNELMODE, TSS_DESC, GRAN4K | PRESENT);
    //load the new table
    gdt_load(GDT,5);

    //jump into the new segments
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


    //initiate the i/o bitmap
    tss.iobmp_offset =(uint32_t)(((uint32_t)&tss.iobmp) - ((uint32_t)&tss)); // calculate real adressoffset
    size_t i;
    for(i=0; i<2048; i++)
        tss.iobmp[i]=0xFFFFFFFF;

    // load the TSS
    asm volatile("ltr %%ax" : : "a" (5 << 3));
}

/**
 * @brief Sets a entry in gdt
 * @param gdt Pointer to the gdt in which the descriptor should be set
 * @param entry The entry number within the GDT
 * @param base Physical base address of the segment (or virtual address of the TSS if type = TSS_DESC)
 * @param size Size of the segment in Bytes (max. 1M addressable) or in 4K blocks if GRAN4K is set (max. 4G addressable)
 * @param prev Previleg level of the memory segment
 * @param type The Descriptor type 
 * @param flags Descriptor flags
 */
void gdt_set_entry(struct gdt_entry *gdt, uint16_t entry, paddr_t base, size_t size, privilege_t prev, gdt_seg_type type, gdt_flag_type flags)
{
    gdt[entry].limit_low =      (uint16_t) size;
    gdt[entry].Base_low =       (uint16_t) base;
    gdt[entry].Base_middle =    (uint8_t) (base >> 16);
    gdt[entry].Access =         (uint8_t) (flags >> 8) | (prev << 5) |type;
    gdt[entry].limit_Flags=     (uint8_t)  flags | (uint8_t)(size >> 16);
    gdt[entry].base_high =      (uint8_t) (base >> 24);
}


/**
 *  @brief Load GDT into register
 *  @param gdt GDT to be loaded
 *  @param last_entry last set GDT entry
 */
void gdt_load(struct gdt_entry *gdt, uint16_t last_entry)
{
    struct gdtpt gdtp;
    gdtp.limit = ((last_entry + 1) * 8) - 1;
    gdtp.base = gdt;
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

/**
 * @brief sets the IO Access bitmap of context
 * @param context the thread context which acces bitmap should be set as current
 */
void set_iobmp(struct arch_thread_context *context)
{
    memcpy(tss.iobmp, context->ports.iobmp, sizeof(tss.iobmp));
}
