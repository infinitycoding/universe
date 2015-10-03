#include <arch.h>

#include <mm/pmm.h>
#include <mm/vmm.h>

void INIT_MM(struct multiboot_struct *mb_info)
{
    INIT_PMM(mb_info);

#ifdef _VMM_
    INIT_VMM();
#endif

    INIT_HEAP();
}

