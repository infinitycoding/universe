#include <universe.h>


uint32_t fork()
{
    uint32_t pid;
    asm volatile("int $128;": "=a"(pid) : "a" (SYS_FORK));
    return pid;
}

// universe stuff

void thread_exit(int retv)
{
  asm volatile("int $112;": : "a" (SYS_THREAD_EXIT) , "b"(retv));
}

void thread_launch(void * function, int argc, void *argv)
{
    asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(SYS_THREAD_LAUNCH) , "b"(function), "c"(argc), "d" (argv), "S" (&thread_exit));
}

uint32_t alloc_memory(int pages)
{
	uint32_t addr;
	asm volatile("int $112;" : "=a" (addr) : "a" (SYS_ALLOC_MEMORY), "b" (pages));
	return addr;
}

uint32_t identify_universe(void)
{
    return universe_syscall(SYS_IDENTIFY_UNIVERSE,0,0,0,0,0);
}


