#include <stdint.h>
#include <stdarg.h>


#define SYS_EXIT 1
#define SYS_FORK 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

#define exit(RETV) asm volatile("int $128" : : "a"(SYS_EXIT), "b"(RETV))

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

#define print(STR) asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(0) , "b"(STR))
#define thread_exit(RETV) asm volatile("int $112;": : "a"(1) , "b"(RETV))
#define thread_launch(FUNCTION, PARAMETER) asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(2) , "b"(FUNCTION), "c"(PARAMETER))
#define identify_os(...) universe_syscall(3,0,0,0,0,0)
#define UNIVERSE_OS 'u' | ('n' << 8) | ('i' << 16)
#define is ^0xFF000000 ==

