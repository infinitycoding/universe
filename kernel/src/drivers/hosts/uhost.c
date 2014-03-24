#include <drivers/hosts/uhost.h>
#include <udrcp.h>
#include <list.h>
#include <drvlst.h>
#include <elf.h>
#include <heap.h>
#include <paging.h>
#include <memory_layout.h>
#include <printf.h>
#include <vfs.h>
#include <drivers/video.h>
#include <udrcpHost.h>
#include <process.h>

/*
TEST IMPLEMENTATION
PLEASE DONT USE IT
I'LL PORT THE USERMODE LIBRARY LATER
*/


list_t *subdrivers;

void INIT_UHOST(int argc, void **argv)
{
    subdrivers = list_create();
    struct multiboot_struct *mb_info =  argv[1];
    char **drv_list =  argv[0];
    int i;
    pckmgr *pman;
    printf("uhost subsystems:\n");
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
        switch(pck->type)
        {
            case RESET_CON:
                printf("host: connection reset\n");
            continue;

            case PING:
                printf("host: recieved ping -> sending pong\n");
                respond(pman,pck->id,PONG,UHOST_DEFAULT_ASYNCHRON_SIZE,UHOST_DEFAULT_ASYNCHRON);
            continue;

        };

        list_next(subdrivers);
        if(list_is_last(subdrivers))
            list_set_first(subdrivers);

    }

    return; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
