#ifndef	_LINUX_LIMITS_H_
#define	_LINUX_LIMITS_H_

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

#ifndef INT8_MIN
#define INT8_MIN -128
#endif

#ifndef INT8_MAX
#define INT8_MAX 127
#endif

#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif


#ifndef INT16_MIN
#define INT16_MIN -32768
#endif

#ifndef INT16_MAX
#define INT16_MAX 32767
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif


#ifndef INT32_MIN
#define INT32_MIN ((int32_t) -0x80000000)
#endif

#ifndef INT32_MAX
#define INT32_MAX 0x7FFFFFFF
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif


#ifndef INT64_MIN
#define INT64_MIN ((int64_t) -0x8000000000000000LL)
#endif

#ifndef INT64_MAX
#define INT64_MAX 0x7FFFFFFFFFFFFFFFLL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX ((uint64_t) -1ULL)
#endif


#ifndef INTPTR_MIN
#define INTPTR_MIN INT32_MIN
#endif

#ifndef INTPTR_MAX
#define INTPTR_MAX INT32_MAX
#endif

#ifndef UINTPTR_MAX
#define UINTPTR_MAX UINT32_MAX
#endif


#ifndef INTMAX_MIN
#define INTMAX_MIN INT64_MIN
#endif

#ifndef INTMAX_MAX
#define INTMAX_MAX INT64_MAX
#endif

#ifndef UINTMAX_MAX
#define UINTMAX_MAX UINT64_MAX
#endif

#define PATH_MAX        4096

#endif