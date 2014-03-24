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
    printf("uhost subsystems:\n");
    pckmgr *pman;
    for(i = 0; drv_list[i]; i++)
    {
        struct mods_add *drv_mod = (struct mods_add *)find_module(mb_info, drv_list[i]);
        if(drv_mod != NULL)
        {
            size_t drv_len = drv_mod->mod_end - drv_mod->mod_start;
            size_t drv_pages = NUM_PAGES(drv_len);
            void *driver = (void*)vmm_automap_kernel_range(current_context,(paddr_t) drv_mod->mod_start, drv_pages, VMM_WRITABLE);
            pman = new_pckmgr(vfs_create_pipe(0, 0), vfs_create_pipe(0, 0), vfs_create_pipe(0, 0));
            load_elf(driver,0,0,&pman->p);
            list_push_front(subdrivers,pman);
            printf("%s: %p\n",drv_list[i], drv_mod);
        }
    }
    printf("\n");
    //Poll packages
    unsigned int buffer[128];
    struct pipeset *p = &pman->p;
    int len = p->stdout->length;

    while(1)
    {
        while(len == p->stdout->length){}
        int readed = p->stdout->length-len;
        printf("host: readed %d bytes\n", readed);
        vfs_read(p->stdout, len, readed, buffer);
        len = p->stdout->length;
        int offset = 0;

        while(offset < (readed/4))
        {
            printf("host: ");
            printf("recieved package %d    size:%d    type:%x\n",buffer[offset],buffer[offset+1],buffer[offset+2]);
            switch(buffer[offset+2])
            {
                case RESET_CON:
                    printf("host: connection reset\n");
                    offset += 3;
                continue;

                case PING:
                    printf("host: recieved ping -> sending pong\n");
                    respond(pman,buffer[offset],PONG,UHOST_DEFAULT_ASYNCHRON_SIZE,UHOST_DEFAULT_ASYNCHRON);
                    offset += buffer[offset+1];
                continue;

                default:
                    offset += buffer[offset+1];
                continue;

            };


        }

    }

    return; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
