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

#include <sys/multiboot.h>

//memory
#include "memory_layout.h"
#include <pmm.h>
#include <mm/paging.h>
#include <mm/heap.h>

//descriptor tables
#include <gdt.h>
#include <idt.h>
#include <pic.h>

//scheduling
#include <sched/elf.h>
#include <sched/scheduler.h>

//vfs
#include <vfs/vfs.h>
#include <vfs/module.h>

//event and timing
#include <clock.h>
#include <event/trigger.h>

//udrcp
#include <udrcp/hypervisor.h>
#include <udrcp/pfp.h>


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
    INIT_VFS();
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
        int diff = (int)modules[i].string - (int)phys;
        modules[i].string = virt + diff;
    }

    struct mapping_statistics stats = map_all(mb_info);
    printf("%d modules total, %d successfully loaded, %d failed\n", stats.total, stats.load_success, stats.load_failed);

    vfs_inode_t *pfnode = vfs_lookup_path("/drivers/system.pf");

    if(pfnode != NULL)
    {
        void *argv[2];
        char *pf = (char *)malloc(pfnode->length+1);
        vfs_read(pfnode, 0, 0, pf, pfnode->length);
        pf[pfnode->length+1] = '\0';
        list_t *pipelines = pfp(pf);
        struct section *sec = list_pop_front(pipelines);

        argv[1] = mb_info;
        argv[0] = sec;
        kernel_thread_create(INIT_HYPERVISOR,(char **) argv, NULL);
    }

    load_elf_from_file(vfs_lookup_path("/drivers/keyboard.elf"), 0, 0, 0, 0, 0);
    load_elf_from_file(vfs_lookup_path("/drivers/cga.elf"), 0, 0, 0, 0, 0);

    vfs_inode_t *testnode = vfs_lookup_path("/ultrashell");

    if(testnode == NULL)
    {
        printf("ultrashell not in vfs\n");
    }
    else
    {
        load_elf_from_file(testnode, 0, 0, 0, 0, 0);
    }

    return 0;
}

