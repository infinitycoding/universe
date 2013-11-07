#ifndef _cpu_h_
#define _cpu_h_

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

struct cpu_state
{
	uint32_t   gs;
	uint32_t   fs;
	uint32_t   es;
	uint32_t   ds;

	uint32_t   edi;
	uint32_t   esi;
	uint32_t   ebp;

	uint32_t   ebx;
	uint32_t   edx;
	uint32_t   ecx;
	uint32_t   eax;

	uint32_t   intr;
	uint32_t   error;

	uint32_t   eip;
	uint32_t   cs;
	uint32_t   eflags;
	uint32_t   esp;
	uint32_t   ss;
};

#define CPU_ARG0 eax
#define CPU_ARG1 ebx
#define CPU_ARG2 ecx
#define CPU_ARG3 edx
#define CPU_ARG4 esi
#define CPU_ARG5 edi

#endif