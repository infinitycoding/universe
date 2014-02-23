#include <drivers/hosts/uhost.h>
#include <udrcp.h>
#include <list.h>
#include <drvlst.h>
#include <elf.h>
#include <heap.h>
#include <paging.h>
#include <memory_layout.h>
#include <printf.h>

list_t *drivers;
void INIT_UHOST(int argc, void **argv)
{
    printf("uhost-init sagt hallo! argc:%d \n", argc);

    struct multiboot_struct *mb_info =  argv[1];
    char **drv_list =  argv[0];
    printf("%s\n",drv_list[0]);

    return; /// i don't know why, but it is necessary to return into kernel_thread_exit()
}
