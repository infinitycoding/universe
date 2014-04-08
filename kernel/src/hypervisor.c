/*
     Copyright 2014 Infinitycoding all rights reserved
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


/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 **/

#include <vfs.h>
#include <multiboot.h>
#include <stdint.h>
#include <list.h>
#include <drvlst.h>
#include <elf.h>
#include <heap.h>
#include <paging.h>
#include <memory_layout.h>
#include <printf.h>
#include <vfs.h>
#include <process.h>
#include <ioport.h>
#include <hypervisor.h>
#include <trigger.h>


#define DEBUG


list_t *subdrivers;
list_t *interrupts;

void subsystem_isr(int irq)
{
    list_set_first(interrupts);
    while(!list_is_last(interrupts) && !list_is_empty(interrupts))
    {
        struct int_relation *rel = list_get_current(interrupts);
        if(rel->intnr == irq)
            send_package(rel->drv, INTSIG, 0, NULL);
        list_next(interrupts);
    }
}

void INIT_HYPERVISOR(int argc, void **argv)
{
    subdrivers = list_create();
    interrupts = list_create();
    struct multiboot_struct *mb_info =  argv[1];
    char **drv_list =  argv[0];
    int i;
    pckmgr *pman;
    printf("hypervisor subsystems:\n");
    for(i = 0; drv_list[i]; i++)
    {
        struct mods_add *drv_mod = (struct mods_add *)find_module(mb_info, drv_list[i]);
        if(drv_mod != NULL)
        {
            size_t drv_len = drv_mod->mod_end - drv_mod->mod_start;
            size_t drv_pages = NUM_PAGES(drv_len);
            void *driver = (void*)vmm_automap_kernel_range(current_context,(paddr_t) drv_mod->mod_start, drv_pages, VMM_WRITABLE);
            pman = new_pckmgr(vfs_create_pipe(0, 0), vfs_create_pipe(0, 0), vfs_create_pipe(0, 0));
            load_elf(driver,0,0,&pman->pset);
            list_push_front(subdrivers,pman);
            printf("%s: %p\n",drv_list[i], drv_mod);
        }
    }
    printf("\n");
    //Poll packages
    if(list_is_empty(subdrivers))
    {
        printf("could not load any subsystem!\n");
        while(1);
    }

    list_set_first(subdrivers);

    /** NOTE: current implementation is based in polling. Switch to pipetrigger as fast as possible**/
    while(1)
    {

        pman = list_get_current(subdrivers);
        pck_t *pck = fetch_pipe(pman);
        if(!pck)
        {
            list_next(subdrivers);
            if(list_is_last(subdrivers))
                list_set_first(subdrivers);
            continue;
        }

        //printf("host: recieved package %d    size:%d    type:%x\n",pck->id,pck->size,pck->type);
        port_type t;
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
                t = hw_port;
                respond(pman,pck->id,SUCCESS,sizeof(port_type),&t);
            break;

            case INT_REQ:
                #ifdef DEBUG
                    printf("host: setting up interrupt signal 0x%x\n",*((unsigned int*)pck->data));
                #endif // DEBUG
                r = malloc(sizeof(struct int_relation));
                r->intnr = *((unsigned int*)pck->data);
                r->drv = pman;
                list_push_front(interrupts,r);
                if(add_int_trigger(r->intnr, NULL,subsystem_isr))
                {
                    printf("sucess!\n");
                   respond(pman,pck->id,SUCCESS,0,NULL);
                }
                else
                {
                    printf("could not allocate interrupt\n");
                    ret = -1;
                    respond(pman,pck->id,ERROR,sizeof(unsigned int),&ret);
                }
            break;


            default:
                printf("host: recieved unknown package %d    size:%d    type:%x\n",pck->id,pck->size,pck->type);
            break;

        };


    }

    return; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
