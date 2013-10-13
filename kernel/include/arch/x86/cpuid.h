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

/**
  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
 */

#ifndef	_cpuid_h_
#define	_cpuid_h_

typedef enum {
	callgate = 0,
	sysenter = 1,
	syscall = 2,
} dynamic_syscall;

typedef enum {
	I386 = 0,
	AMD64 = 1,
	IA32 = 2,
	IA64 = 3,
} arch;

typedef enum{
    primary = 0,
    override = 1,
    secondary = 2,
    reserved = 3,
}processor_type;

//Flagblock0

#define SSE3    1
#define DTES64  4
#define MONITOR 8 //MONITOR & MWAYT SUPPORT
#define DS_CPL  16
#define SMX     32
#define VMX     64
#define EST     128
#define TM2     256
#define SSSE3   512
#define CNXT_ID 1024
#define CX16    8192
#define XTPR    16384
#define PDCM    32768
#define DCA     262288
#define SSE4_1  524576
#define SSE4_2  1049152
#define X2APIC  2098304
#define MOVBE   4196608
#define POPCNT  8393216
#define XSAVE   67145728
#define OSXSAVE 134291456

//Flagblock1

#define FPU     1
#define VME     2
#define DE      4
#define PSE     8
#define TSC     16
#define MSR     32
#define PAE     64
#define MCE     128
#define CX8     256
#define APIC    512
#define SEP     2048
#define MTRR    4096
#define PGE     8192
#define MCA     16384
#define CMOV    32786
#define PAT     65536
#define PSE36   131072
#define PSN     262144
#define CLFLSH  524288
#define DS      2097152
#define ACPI    4194304
#define MMX     8388608
#define FXSR    16777216
#define SSE     33554432
#define SSE2    67108864
#define SS      134217728
#define HTT     268435456
#define TM      536870912
#define IA64    1073741824
#define PBE     2147483648



//128 bytes
struct cpu_properties {
	bool cpuid_support;
	uint32_t manufactory;
	uint32_t max_std_func;
	uint32_t max_spec_func;

	char vendor_id[13];
	uint8_t family;
	uint8_t model;
	uint8_t stepping;
	uint8_t type;
	uint8_t brandID;
	uint8_t clflush;
	uint8_t logic_cores;
	bool LM;


	dynamic_syscall dsysc;
	uint16_t ext_brandID;

	uint8_t APIC_ID;

	arch architecture;


	char cpu_type[49];
	//cpuid 1 infos
	uint32_t flagblock0;
	uint32_t flagblock1;
	//empty space for futur
};

struct cpuid_regs {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
};


void INIT_CPUID(void);
void CPU_info(struct cpu_properties *cpu);
int identify_cpu(struct cpu_properties*);

#endif
