#include <stdint.h>
#include <stdarg.h>


#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

inline uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
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

#define exit(RETV) asm volatile("int $128" : : "a"(SYS_EXIT), "b"(RETV))
#define thread_exit(RETV) asm volatile("int $112;": : "a"(0) , "b"(STR))

inline uint32_t universe_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
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

#define print(STR) asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(1) , "b"(STR))
