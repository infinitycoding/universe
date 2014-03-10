#include <drivers/hosts/uhost.h>
#include <udrcp.h>
#include <list.h>
#include <drvlst.h>
#include <elf.h>
#include <heap.h>
#include <paging.h>
#include <memory_layout.h>
#include <printf.h>


list_t *subdrivers;

void INIT_UHOST(int argc, void **argv)
{
    subdrivers = list_create();
    struct multiboot_struct *mb_info =  argv[1];
    char **drv_list =  argv[0];
    int i;
    printf("uhost subsystems:\n");
    for(i = 0; drv_list[i]; i++)
    {
        struct mods_add *drv_mod = (struct mods_add *)find_module(mb_info, drv_list[i]);
        if(drv_mod != NULL)
        {
            size_t drv_len = drv_mod->mod_end - drv_mod->mod_start;
            size_t drv_pages = NUM_PAGES(drv_len);
            void *driver = (void*)vmm_automap_kernel_range(current_context,(paddr_t) drv_mod->mod_start, drv_pages, VMM_WRITABLE);
            load_elf(driver,0,0);
            printf("%s: %#08x\n",drv_list[i], drv_mod);
        }
    }
    while(1);
    return; /// i don't know why, but it is necessary to return into kernel_thread_exit(). Without a return command the function will cause a memory access exception
}
