#ifndef	_STDINT_H_
#define	_STDINT_H_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.
 
     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#define TRUE			1
#define true			1
#define FALSE			0
#define false			0
#define NULL 			(void *)0

typedef unsigned char     	uint8_t;
typedef unsigned short    	uint16_t;
typedef unsigned int      	uint32_t;
typedef unsigned long long 	uint64_t;

typedef signed char		int8_t;
typedef signed short 		int16_t;
typedef signed int 		int32_t;
typedef signed long long 	int64_t;

typedef unsigned int 		uintptr_t;
typedef long int 		intptr_t;

typedef int64_t 		intmax_t;
typedef uint64_t 		uintmax_t;

typedef unsigned int 		size_t;

typedef unsigned int 		paddr_t;
typedef unsigned int 		vaddr_t;

typedef int 				bool;

#endif
