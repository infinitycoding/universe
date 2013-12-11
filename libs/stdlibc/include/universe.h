#ifndef UNIVERSE_H
#define UNIVERSE_H



#include <stdint.h>
#include <stdarg.h>


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
#define SYS_PIPE 11 // TODO

#define UNIVERSE_OS ('u' | ('n' << 8) | ('i' << 16))

uint32_t linux_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t universe_syscall(uint32_t function, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t fork();
uint32_t alloc_memory(int pages);
void thread_exit(int retv);
void exit(int retv);
uint32_t identify_universe(void);
void thread_launch(void * function, int argc, void *argv);
void print(char *str);
#endif
