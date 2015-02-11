Hardware Abstraction Layer Specification
========




start
-----------
The global function start is the entry function to the kernel. It must setup all functionality to
run c code in the memorylayout given in memorylayout.h. After that's done, the start function has
to call the main function of the Kernel. After the main  function returned, the start function must halt or shut down the system. Normaly this function is provided in the boot.asm file.


Previlege Level
-----------
If your hanrdware does support different software previleg-levels (like x86 does) the following
functions have to be implemented:

###INIT_PREV(void)
Initialation of the previleg system. It's the first function called in kernel main function.




Physical Memory Management (PMM)
-----------
This module provides the capabilities to manage the physical installed RAM. 

###INIT_PMM
Initiates the PMM module

###page_size

###pmm_alloc_page

###pmm_alloc_page_range

###pmm_mark_page_as_free

###pmm_alloc_page_limit

###pmm_mark_page_as_free

###pmm_check_page_range

###pmm_check_page


Interrupts/Exceptions
-----------
The interrupt module handles hanrdware interrupts and exceptions and relays syscalls
to the syscall module if it's required by the platform. If any exceptions occure report them
to the error module and do a kernel panic if the exception can't be handled.


###INIT_INTERRUPT()
Initiates the Interrupt module

###interrupts_enable()
Enables the reception of hanrdware interrupts.

###interrupts_disable()
Disables the reception of hanrdware interrupts.



Virtual Memory Management(VMM)/Paging
-----------


Process Contexts
-----------
This module provides the ability to create, switch and destroy threads. This functions should
manage the cpu states, memory contexts, calling arguments and process previles. This module has
to be implemented even if the cpu does not provides multitasking or VMM. In this case 
arch_create_thread_context could be realizes as setting a pointer to a programm image and
arch_switch_thread_context as calling the programms main function.

###arch_create_thread_context
creates a thread context

###arch_destroy_thread_context
destroys a thread context

###arch_switch_thread_context


Basical Output
-----------

Debuging/Dumping/errorhandling
-----------

clock/timing routines
-----------


CPU Features
-----------

