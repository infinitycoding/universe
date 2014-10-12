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
#include <list.h>
#include <printf.h>
#include <printf.h>
#include <event/trigger.h>

#include <udrcp/pfp.h>
#include <udrcp/ioport.h>
#include <udrcp/memory.h>
#include <udrcp/hypervisor.h>

#include <sched/elf.h>
#include <sched/process.h>

#include <mm/heap.h>
#include <mm/paging.h>
#include <sys/multiboot.h>
#include <vfs/vfs.h>
#include <pmm.h>




//#define DEBUG


list_t *subdrivers;
list_t *interrupts;

void subsystem_isr(struct cpu_state **cpu)
{
    iterator_t int_it = iterator_create(interrupts);
    while(!list_is_last(&int_it) && !list_is_empty(interrupts))
    {
        struct int_relation *rel = list_get_current(&int_it);
        if(rel->intnr == (*cpu)->intr-32)
        {
            send_package(rel->drv, INTSIG, 0, NULL);
        }
        list_next(&int_it);
    }
}

int INIT_HYPERVISOR(int argc, char **argv, char **environ)
{
    subdrivers = list_create();
    interrupts = list_create();
    struct section *current_section = (struct section *)argv[0];
    pckmgr *pman;
    printf("hypervisor subsystems:\n");
    iterator_t i = iterator_create(current_section->subtree);
    while(!list_is_last(&i))
    {
        char *path = ((struct pnode *)list_get_current(&i))->file;
        vfs_inode_t *driver_inode = vfs_lookup_path(path);

        if(driver_inode != NULL)
        {
            printf("load %s\n", path);
            pman = new_pckmgr(vfs_create_pipe(0, 0), vfs_create_pipe(0, 0), vfs_create_pipe(0, 0));

            struct driver *new_driver = malloc(sizeof(struct driver));
            new_driver->pman = pman;
            new_driver->process = load_elf_from_file(driver_inode, 0, 0, &pman->pset, 0, NULL, NULL);
            new_driver->ports = list_create();
            new_driver->memory = list_create();
            list_push_front(subdrivers,new_driver);
        }
        list_next(&i);
    }
    printf("\n");
    //Poll packages
    if(list_is_empty(subdrivers))
    {
        printf("could not load any subsystem!\n");
        while(1);
    }

    iterator_t subdriver_it = iterator_create(subdrivers);

    /** NOTE: current implementation is based on polling. Switch to pipetrigger as fast as possible**/

    while(1)
    {
        struct driver *current_driver = list_get_current(&subdriver_it);
        pman = current_driver->pman;
        pck_t *pck = fetch_pipe(pman);
        if(!pck)
        {
            list_next(&subdriver_it);
            if(list_is_last(&subdriver_it))
                list_set_first(&subdriver_it);
            continue;
        }

        //printf("host: recieved package %d    size:%d    type:%x\n",pck->id,pck->size,pck->type);
        struct int_relation *r;
        int ret;
        switch(pck->type)
        {
            case RESET_CON:
#ifdef DEBUG
                printf("host: connection reset\n");
#endif
                break;

            case PING:
#ifdef DEBUG
                printf("host: recieved ping -> sending pong\n");
#endif
                respond(pman,pck->id,PONG,UHOST_DEFAULT_ASYNCHRON_SIZE,UHOST_DEFAULT_ASYNCHRON);
                break;

            case PORT_ALLOC:
#ifdef DEBUG
                printf("host: allocationg Port 0x%x\n",*((unsigned int*)pck->data));
#endif
                handle_port_alloc(current_driver, pck);
                break;

            case INT_REQ:
#ifdef DEBUG
                printf("host: setting up interrupt signal 0x%x\n",*((unsigned int*)pck->data));
#endif // DEBUG
                r = malloc(sizeof(struct int_relation));
                r->intnr = *((unsigned int*)pck->data);
                r->drv = pman;
                list_push_front(interrupts,r);
                add_trigger(WAIT_INT, r->intnr, false, NULL, subsystem_isr);
                respond(pman,pck->id,SUCCESS,0,NULL);
                break;

            case PMA_ALLOC:
#ifdef DEBUG
                printf("PMA-Alloc\n");
#endif
                handle_pma_alloc(current_driver, pck);

                break;

            case PMA_FREE:
                printf("PMA-Free\n");
                handle_pma_free(current_driver, pck);
                break;



            default:
                printf("host: recieved unknown package %d    size:%d    type:%x\n",pck->id,pck->size,pck->type);
                break;

        };


    }

    return 0; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
