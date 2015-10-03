#ifndef _stddef_h_
#define _stddef_h_

#include <stdint.h>
#define offsetof(type, field) ((size_t)(&((type *)0)->field))

#endif