#ifndef _VERSION_H_
#define _VERSION_H_

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


// version
#define VERSION "1.0"
#define RELEASE "Devel"

// arch
#define ARCH_X86      0x0
#define ARCH_AMD64   0x1
#define ARCH_ARM      0x2
#define ARCH_POWER_PC 0x3
#define ARCH_SPARC    0x4

#define ARCH ARCH_X86

#if ARCH == ARCH_X86
	#define ARCH_STRING "X86"
#endif
#if ARCH == ARCH_AMD64
	#define ARCH_STRING "amd64"
#endif
#if ARCH == ARCH_ARM
	#define ARCH_STRING "ARM"
#endif
#if ARCH == ARCH_POWER_PC
	#define ARCH_STRING "Power PC"
#endif
#if ARCH == ARCH_SPARC
	#define ARCH_STRING "Sparc"
#endif

#endif

