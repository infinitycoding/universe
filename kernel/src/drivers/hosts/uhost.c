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
    struct pipeset *p = malloc(sizeof(struct pipeset));
    for(i = 0; drv_list[i]; i++)
    {
        struct mods_add *drv_mod = (struct mods_add *)find_module(mb_info, drv_list[i]);
        if(drv_mod != NULL)
        {
            size_t drv_len = drv_mod->mod_end - drv_mod->mod_start;
            size_t drv_pages = NUM_PAGES(drv_len);
            void *driver = (void*)vmm_automap_kernel_range(current_context,(paddr_t) drv_mod->mod_start, drv_pages, VMM_WRITABLE);

            p->stdin = vfs_create_pipe(0, 0);
            p->stdout = vfs_create_pipe(0, 0);
            p->stderr = vfs_create_pipe(0, 0);
            load_elf(driver,0,0,p);
            list_push_front(subdrivers,p);
            printf("%s: %p\n",drv_list[i], drv_mod);
        }
    }
    //Poll packages
    unsigned int buffer[128] = {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
    unsigned int pong[4] = {1,16,1,'1'|(':'<<8)|('0'<<16)|('0'<<24)};
    int len = p->stdout->length;
    while(1)
    {
        //printf("read %d\n",p->stdout->length);
        while(len == p->stdout->length){}
        int readed = p->stdout->length-len;
        set_color(BLUE);
        printf("host: ");
        set_color(WHITE);
        printf("readed %d bytes\n", readed);
        vfs_read(p->stdout, len, readed, buffer);
        len = p->stdout->length;
        int offset = 0;

        while(offset < (readed/4))
        {
            set_color(BLUE);
            printf("host: ");
            set_color(WHITE);
            printf("recieved package %d    size:%d    type:%x\n",buffer[offset],buffer[offset+1],buffer[offset+2]);
            if(buffer[offset+2] == 0)
            {
                set_color(BLUE);
                printf("host: ");
                set_color(WHITE);
                printf("reset\n");
                offset += 3;
            }
            else if(buffer[offset+2] == 1)
            {
                set_color(BLUE);
                printf("host: ");
                set_color(WHITE);
                printf("recieved ping\n");
                set_color(BLUE);
                printf("host: ");
                set_color(WHITE);
                printf("send pong\n");
                vfs_write(p->stdin,p->stdin->length,pong,16);
                offset += buffer[offset+1];
            }
        }



    }
    return; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
