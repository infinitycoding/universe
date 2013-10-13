#ifndef _version_h_
#define _version_h_

// version
#define VERSION "2.0"
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

