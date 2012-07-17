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



	 Diese Datei ist ein Teil vom Universe Kernel.

	 Das Universe Kernel ist Freie Software: Sie koennen es unter den Bedingungen
	 der GNU General Public License, wie von der Free Software Foundation,
	 Version 3 der Lizenz oder jeder sp‰teren
	 veroeffentlichten Version, weiterverbreiten und/oder modifizieren.

	 Das Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 Universe Kernel wird in der Hoffnung, dass es nuetzlich sein wird, aber
	 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
	 Gew‰hrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN ZWECK.
	 Siehe die GNU General Public License fuer weitere Details.

	 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

 /*
    authors:
    -Simon Diepold aka. Tdotu (Universe Team) simon.diepold@infinitycoding.de
 */

#ifndef	_cpuid_h_
#define	_cpuid_h_
	static char* cpu_manufactorys[]={
	"AMD", "AMD", "Intel", "Centaur",
	"Cyrix", "Transmeta", "Transmeta","National Semiconductor",
	"NexGen", "Rise", "SiS", "UMC",
	"VIA", "Vortex", "unknown"
	};

	static char* vendorID[]={
	"AuthenticAMD",	"AMDisbetter!", "GenuineIntel", "CentaurHauls",
	"CyrixInstead", "TransmetaCPU", "GenuineTMx86", "Geode by NSC",
	"NexGenDriven", "RiseRiseRise", "SiS SiS SiS ", "UMC UMC UMC ",
	"VIA VIA VIA ", "Vortex86 SoC"
	};
	typedef enum {
	callgate =0,
	sysenter =1,
	syscall =2,
	}dynamic_syscall;

	struct cpu_properties{
	    bool cpuid_support;
		uint32_t manufactor; //position in cpu_manufactorys array
		uint32_t family;
		uint32_t model;
		uint32_t stepping;
		uint8_t brandID;
		uint16_t ext_brandID;
		uint32_t max_std_func;
		uint32_t max_spec_func;
		dynamic_syscall dsysc;
		char cpu_type[48];
	};

	struct cpuid_regs{
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	};

	static void cpuid(uint32_t function,struct cpuid_regs* output){
		asm volatile("cpuid":"=a" (output->eax), "=b"(output->ebx),"=c"(output->ecx),"=d"(output->edx):"a"(function));
	}

	static char* brandID_Intel[]={
    "Intel Celeron", "Intel Pentium III", "Intel Pentium III Xeon",
    "Intel Pentium III", "Mobile Intel Pentium III", "Mobile Intel Celeron",
    "Intel Pentium 4", "Intel Pentium 4", "Intel Celeron",
    "Intel Xeon", "Intel Xeon MP", "Intel Pentium 4M",
    "Mobile Intel Celeron", "Mobile Genue Intel", "Intel Mobile Celeron M",
    "Mobile Intel Celeron", "Intel Celeron", "Mobile Genue Intel",
    "Intel Pentium M", "Mobile Intel Celeron"
	};

    //only K8 DDR1
	static char* brandID_AMD[]={
    "AMD engeneer sample", "","", "","AMD Athlon 64 XX00+", "AMD Athlon 64 X2 XX00+",
    "AMD Athlon 64 FX-ZZ", "", "AMD Athlon 64 XX00+ mobile", "AMD Athlon 64 XX00+ mobile low power", "AMD Turion 64 ML-XX", "AMD Turion 64 MT-XX",
    "AMD Opteron 1YY", "AMD Opteron 1YY", "AMD Opteron 1YY HE", "AMD Opteron 1YY EE", "AMD Opteron 2YY", "AMD Opteron 2YY",
    "AMD Opteron 2YY HE", "AMD Opteron 2YY EE", "AMD Opteron 8YY", "AMD Opteron 8YY", "AMD Opteron 8YY HE", "AMD Opteron 8YY EE",
    "AMD Athlon 64 EE00+", "", "", "", "", "AMD Athlon XP-M XX00+ mobile",
    "AMD Athlon XP-M XX00+ mobile low power", "", "AMD Athlon XP XX00+", "AMD Sempron TT00+ mobile", "AMD Sempron TT00+", "AMD Sempron TT00+ mobile low power",
    "AMD Athlon 64 FX-ZZ", "", "AMD Sempron X64 TT00+", "", "", "AMD Opteron DC 1RR SE",
    "AMD Opteron DC 2RR SE", "AMD Opteron DC 8RR SE", "AMD Opteron DC 1RR", "AMD Opteron DC 1RR", "AMD Opteron DC 1RR HE", "AMD Opteron DC 1RR EE",
    "AMD Opteron DC 2RR", "AMD Opteron DC 2RR", "AMD Opteron DC 2RR HE", "AMD Opteron DC 2RR EE", "AMD Opteron DC 8RR", "	AMD Opteron DC 8RR",
    "AMD Opteron DC 8RR HE", "AMD Opteron DC 8RR EE", "AMD Opteron DC 1RR", "AMD Opteron DC 2RR", "AMD Opteron DC 8RR", "AMD Opteron DC 1RR",
    "AMD Opteron DC 2RR", "	AMD Opteron DC 8RR", "unknown AMD X86 processor"
	};

#endif
