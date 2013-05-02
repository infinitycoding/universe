#ifndef _thread_h_
#define _thread_h_


/*
	Copyright 2012 universe coding group (UCG) all rights reserved
	This file is part of the Universe Kernel.

	Universe Kernel is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Universe Kernel is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Universe Kernel.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>

#define THREAD_ACTIV 1
#define THREAD_FREEZED 2
#define THREAD_KERNELMODE 4
#define THREAD_NORMAL_PRIORITY 8
#define THREAD_REALTIME_PRIORITY 16
#define THREAD_ZOMBIE 32
#define THREAD_STACK_SIZE 4096

#define MAIN_THREAD 64
#define ZOMBIE 128

typedef uint32_t tid_t;

struct thread
{
    struct thread *prev;
    struct thread *next;
    struct cpu_state *thread_state;
    uint32_t flags;
    int return_value;
    tid_t tid; //Thread ID
};



struct zombietid
{
    struct zombietid *prev;
    struct zombietid *next;
    tid_t tid;
};




#endif
