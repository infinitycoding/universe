#include <stdint.h>
#include <stdarg.h>

/*
uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi)
{
    asm volatile(
    "push %%ebx;"
    "push %%esi;"
    "push %%edi"
    "int $128"
    "pop %%edi"
    "pop %%esi"
    "pop %%ebx"
    : "=a"


}*/

#define exit(RETV) asm volatile("int $128" : : "a"(0), "b"(RETV))
#define thread_exit(RETV) asm volatile("int $112;": : "a"(0) , "b"(STR))
#define print(STR) asm volatile("pushl %%ebx; int $112; popl %%ebx;": : "a"(1) , "b"(STR))
