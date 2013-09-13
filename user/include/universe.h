#include <stdint.h>
#include <stdarg.h>



#define exit(RETV) asm volatile("int $128" : : "a"(1), "b"(RETV))
#define print(STR) asm volatile("pushl %%ebx; int $128; popl %%ebx;": : "a"(0) , "b"(STR))
