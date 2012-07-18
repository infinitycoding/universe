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

	typedef enum {
        callgate =0,
        sysenter =1,
        syscall =2,
	}dynamic_syscall;

	typedef enum {
        X86 = 0,
        X64 = 1,
	}arch;

	typedef enum {
        no_sse = 0,
        sse = 1,
        sse2 = 2,
        sse3 = 3,
        sse41 = 4,
        sse42 = 5,
	}SSE;

	// CPU Propertie Flags
    #define ACPI 1
    #define Hyper_threading 2
    #define MMX 4
    #define PSE 8 //Page size extension
    #define LAPIC 16
    #define AMD3DNOW 32

	struct cpu_properties{
	    bool cpuid_support;
		uint32_t manufactor; //position in cpu_manufactorys array
		uint8_t brandID;
		uint16_t ext_brandID;
		uint32_t max_std_func;
		uint32_t max_spec_func;
		dynamic_syscall dsysc;
		arch architecture;
		SSE sse_support;
		uint32_t flag;
		uint8_t logic_cores;
        uint8_t APIC_ID;
		char cpu_type[48];
	};

	struct cpuid_regs{
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	};


	int INIT_CPUID(void);
	void CPU_info(void);

#endif
