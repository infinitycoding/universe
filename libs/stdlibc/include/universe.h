#ifndef UNIVERSE_H
#define UNIVERSE_H



#include <stdint.h>
#include <stdarg.h>

// Linux Syscalls
#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_WAITPID 7
#define SYS_CREAT 8
#define SYS_LINK 9
#define SYS_UNLINK 10
//#define SYS_EXECVE 11
#define SYS_CHDIR 12
#define SYS_TIME 13
#define SYS_PIPE 14 // TODO
#define SYS_READDIR 15 // at 141 TODO
#define SYS_GETDENTS 15 // at 141 TODO

static inline uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
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


//Universe Syscalls
#define SYS_THREAD_EXIT 0
#define SYS_THREAD_LAUNCH 1
#define SYS_ALLOC_MEMORY 2
#define SYS_IDENTIFY_UNIVERSE 3
#define SYS_PIPE_TRIGGER 4

static inline uint32_t universe_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
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




uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t universe_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t fork();
uint32_t alloc_memory(int pages);
void thread_exit(int retv);
void exit(int retv);
uint32_t identify_universe(void);
#define UNIVERSE_OS ('u' | ('n' << 8) | ('i' << 16))
void thread_launch(void * function, int argc, void *argv);
#endif
