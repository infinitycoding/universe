#include <drivers/hosts/uhost.h>
#include <udrcp.h>
#include <list.h>
#include <drvlst.h>
#include <elf.h>
#include <heap.h>
#include <paging.h>
#include <memory_layout.h>

list_t *drivers;
void INIT_UHOST(int argc, void **argv)
{
    printf("uhost-init sagt hallo!\n");
    while(1);
    /*
    if(argc < 2)
        return;

    struct multiboot_struct *mb_info =  argv[0];
    char **drv_list =  argv[1];


    list_t *drivers = list_create();
    int index;
    while(drv_list[index] != NULL)
    {

        struct mods_add* module = find_module(mb_info, drv_list[index]);
        if(module)
        {

            struct uhost_driver *current_driver = malloc(sizeof(struct uhost_driver));
            current_driver->stdin   = vfs_create_pipe();
            current_driver->stdout = vfs_create_pipe();
            current_driver->stderr = vfs_create_pipe();
            current_driver->last_packageID = 0;
            list_push_front(drivers,current_driver);
            size_t len = module->mod_end - module->mod_start;
            size_t pages = NUM_PAGES(len);
            void *mod = (void*)vmm_automap_kernel_range(current_context,(paddr_t) module->mod_start, pages, VMM_WRITABLE);
            load_elf(mod, current_driver->stdin, current_driver->stdout, current_driver->stderr);
        }

        index++;

    }*/
}
