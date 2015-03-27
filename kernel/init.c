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
 * @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
 * @author Michael Sippel <micha@infinitycoding.de>
 * @author Christian Klahn <christian.klahn1@gmail.com>
 * @author Tom Slawik <tom.slawik@gmail.com>
 */


//abstract
#include <printf.h>
#include <logo.h>
#include <panic.h>

#include <multiboot.h>

//memory
#include "memory_layout.h"
#include <pmm.h>
#include <mm/paging.h>
#include <mm/heap.h>
#include <mm/shm.h>

//descriptor tables
#include <gdt.h>
#include <idt.h>
#include <pic.h>

//scheduling
#include <sched/elf.h>
#include <sched/scheduler.h>

//event and timing
#include <clock.h>
#include <event/trigger.h>


/**
* Initalize the Kernel
*
* @param mb_info The pointer to the multiboot-struct from the bootloader
* @param magic_number Multiboot-magic
*
* @return 0
*/
extern struct process_state *kernel_state;
extern list_t *running_threads;
int init (struct multiboot_struct *mb_info, uint32_t magic_number)
{
    clear_screen();
    if (magic_number != 0x2BADB002)
    {
        panic("Incompatible Bootloader");
    }

    //Init Kernelmodules
    INIT_PREV();
    INIT_PMM(mb_info);
    INIT_PAGING(mb_info);
    INIT_HEAP();
    INIT_PIC();
    INIT_IDT();
    INIT_TRIGGER();
    INIT_CLOCK(500);
    INIT_SCHEDULER();

    //print Logo and loading message
    print_logo(YELLOW);
    set_color(WHITE | (BLACK << 4));
    puts("Universe wird gestartet...\n");
    // count free memory and display it
    uint32_t pages = pmm_count_free_pages();
    printf("%u freie Speicherseiten (%u MB)\n", pages, pages >> 8);

    //print current timestamp
    struct time t;
    update_time(&t);
    printf("\n");
    printf("Timestamp:%d\n\n",unix_time(&t));
    // mapping strings
    int i;
    struct mods_add* modules = (struct mods_add*) mb_info->mods_addr;

    void *phys = (void*)((int)modules[0].string & (int)~0xfff);
    void *virt = (void*) vmm_automap_kernel(current_context, (paddr_t)phys, VMM_WRITABLE);
    for(i = 0; i < mb_info->mods_count; i++)
    {
        struct mods_add *module = &modules[i];

        int diff = (int)module->string - (int)phys;
        module->string = virt + diff;

        // map and execute (all)
        if( i > 0 )
        {
            printf("execute %s\n", module->string);
            int size = module->mod_end - module->mod_start;
            int pages = NUM_PAGES(size);
            vaddr_t virtaddr = vmm_automap_kernel_range(current_context, module->mod_start, pages, VMM_WRITABLE);

            load_elf(virtaddr, module->string, 0, 0, 0, 0);
        }
    }

    //struct mapping_statistics stats = map_all(mb_info);

    return 0;
}


