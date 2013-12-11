#include <universe.h>



uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    uint32_t retv = 0;
    asm volatile(
    "push %ebx;"
    "push %esi;"
    "push %edi"
    );

    asm volatile(
    "int $128;"
    "pop %%edi;"
    "pop %%esi;"
    "pop %%ebx;"
    : "=a" (retv) : "a" (function),  "b" (ebx), "c"(ecx), "d"(edx), "S"(esi), "D"(edi));

    return retv;
}


uint32_t fork()
{
    uint32_t pid;
    asm volatile("int $128;": "=a"(pid) : "a" (SYS_FORK));
    return pid;
}

uint32_t alloc_memory(int pages)
{
	uint32_t addr;
	asm volatile("int $112;" : "=a" (addr) : "a" (4), "b" (pages));
	return addr;
}


uint32_t universe_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    uint32_t retv = 0;
    asm volatile(
    "push %ebx;"
    "push %esi;"
    "push %edi"
    );

    asm volatile(
    "int $112;"
    "pop %%edi;"
    "pop %%esi;"
    "pop %%ebx;"
    : "=a" (retv) : "a" (function),  "b" (ebx), "c"(ecx), "d"(edx), "S"(esi), "D"(edi));

    return retv;
}

void thread_exit(int retv)
{
  asm volatile("int $112;": : "a" (1) , "b"(retv));
}

void exit(int retv)
{
    asm volatile("int $128" : : "a"(SYS_EXIT), "b"(retv));
}

uint32_t identify_universe(void)
{
    return universe_syscall(3,0,0,0,0,0);
}

void thread_launch(void * function, int argc, void *argv)
{
    asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(2) , "b"(function), "c"(argc), "d" (argv), "S" (&thread_exit));
}

void print(char *str)
{
    asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(0) , "b"(str));
}
