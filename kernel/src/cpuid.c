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


 /*
	authors:
	-Simon Diepold aka. Tdotu (Universe Team) simon.diepold@infinitycoding.de

	TODO:
	- replace XX,YY,ZZ,TT,RR,EE in AMD Namestrings (http://www.sandpile.org/x86/cpuid.htm)
	- CPU identification via model, name, stepping...

 */

#include <stdint.h>
#include <cpuid.h>
#include <string.h>
#include <printf.h>

char* brandID_Intel[]={
   "Intel Celeron", "Intel Pentium III", "Intel Pentium III Xeon",
   "Intel Pentium III", "Mobile Intel Pentium III", "Mobile Intel Celeron",
   "Intel Pentium 4", "Intel Pentium 4", "Intel Celeron",
   "Intel Xeon", "Intel Xeon MP", "Intel Pentium 4M",
   "Mobile Intel Celeron", "Mobile Genue Intel", "Intel Mobile Celeron M",
   "Mobile Intel Celeron", "Intel Celeron", "Mobile Genue Intel",
   "Intel Pentium M", "Mobile Intel Celeron"
};

	//only K8 DDR1
char* brandID_AMD[]={
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

static char* vendorID[]={
	"AuthenticAMD",	"AMDisbetter!", "GenuineIntel", "CentaurHauls",
	"CyrixInstead", "TransmetaCPU", "GenuineTMx86", "Geode by NSC",
	"NexGenDriven", "RiseRiseRise", "SiS SiS SiS ", "UMC UMC UMC ",
	"VIA VIA VIA ", "Vortex86 SoC"
};

static char* cpu_manufactorys[]={
	"AMD", "AMD", "Intel", "Centaur",
	"Cyrix", "Transmeta", "Transmeta","National Semiconductor",
	"NexGen", "Rise", "SiS", "UMC",
	"VIA", "Vortex", "unknown"
};

static char* scall[]={"Callgate","Sysenter","Syscall"};
static char* architecture[]={"X86","X64"};



struct cpu_properties current_CPU;

/*
 * Inline function of CPUID
 * @param uint32_t cpuid function number
 * @param pointer to output register structure
 * @return sucess (0=sucess, 1=CPUID is not supported)
 */
static void cpuid(uint32_t function,struct cpuid_regs* output) {
	asm volatile("cpuid":"=a" (output->eax), "=b"(output->ebx),"=c"(output->ecx),"=d"(output->edx):"a"(function));
}


/*
 * Identify curret CPU
 * @param pointer to 128byte free bytes for cpu_prpoerties struct
 * @return sucess (0=sucess, 1=CPUID is not supported)
 */
int identify_cpu(struct cpu_properties *cpu) {
	int ref,eflags;
	asm volatile(
		"pushfl;"
		"pop %%ecx;"
		"mov %%ecx,%%eax;"
		"xor $0x200000,%%eax;"
		"push %%eax;"
		"popfl;"
		"pushfl;"
		"pop %%eax;"
		:"=a"(eflags), "=c"(ref):
	);
	if (eflags==ref) { //cpuid is not supported
	    cpu->cpuid_support=false;
	    return 1;
	} else {
		cpu->cpuid_support=true;
	}

	struct cpuid_regs reg;
	uint32_t i;
	// Copy Vebdir String
	cpuid(0,&reg);
	strncpy(cpu->vendor_id,(char*)&reg.ebx,4);
	strncpy(cpu->vendor_id+4,(char*)&reg.edx,4);
	strncpy(cpu->vendor_id+8,(char*)&reg.ecx,4);
	for (i=0;strncmp(cpu->vendor_id,vendorID[i],12)&& i<15;i++) {}
	cpu->manufactory=i;

	cpu->max_std_func=reg.eax;
	cpuid(0x80000000,&reg);
	cpu->max_spec_func=reg.eax;

	cpuid(1,&reg);
	cpu->family = (reg.eax & 0x00000F00)>>8;
	cpu->model = (reg.eax & 0x000000F0)>>4;
	cpu->stepping = reg.eax & 0x0000000F;
	cpu->ext_family= (reg.eax & 0x0FF00000)>>20;
	cpu->ext_model= (reg.eax &  0x000F0000)>>16;
	cpu->brandID = (uint8_t)reg.ebx;

	//Detect Dynamic Syscall
	if ((!(reg.edx & 0x800)) && !((cpu->family == 6) && (cpu->model < 3) && (cpu->stepping < 3))) {
		cpu->dsysc=sysenter;
	} else if (cpu->max_spec_func>0x80000000) {
		cpuid(0x80000001,&reg);
		if (reg.edx& 0x1000) {
			cpu->dsysc=syscall;
		}
	}

	//get extended BrandID
	if (cpu->max_spec_func>0x80000000) {
		cpuid(0x80000001,&reg);
		cpu->ext_brandID=(uint16_t)reg.ebx;
	} else {
		cpu->ext_brandID=0x3e;
	}


	//Get CPU name/series
	if (cpu->max_spec_func>0x80000004) { // via Brand String
		for (i=0x80000002;i<=0x80000004;i++) {
			cpuid(i,&reg);
			memcpy((cpu->cpu_type+(i-0x80000002)*16),((void*)&reg),16);
		}
	} else if (cpu->brandID) { //via Brand ID
		if (cpu->manufactory==2) { //Intel CPU
			strcpy(cpu->cpu_type,brandID_Intel[cpu->brandID-1]);
		}
	} else if (cpu->manufactory<2) { //AMD CPU
		int ID=(cpu->ext_brandID>>6)& 0x3ff;
		if (ID>0x3e) {ID=0x3e;}
		//uint8_t NN=current_CPU.ext_brandID &0x3F;
		//TODO: replace XX,YY,ZZ,TT,RR,EE
		strcpy(cpu->cpu_type,brandID_AMD[ID]);

	} else { //standart name
		strcpy(cpu->cpu_type,cpu_manufactorys[cpu->manufactory]);
		strcat(cpu->cpu_type,architecture[cpu->architecture]);
		strcat(cpu->cpu_type," CPU");
	}
	// TODO: identify CPU through family,model,stepping, socket

	cpuid(1,&reg);

	cpu->APIC_ID = (uint8_t) (reg.ebx>>24);
	cpu->logic_cores= (uint8_t)(reg.ebx & 0xFF0000)>>16;
	if (reg.edx&(1<<30)) {
		cpu->architecture=X64;
	}
	cpu->cflush_size=(uint8_t)(reg.ebx & 0xFF00)>>8;

	cpu->flagblock0=reg.ecx;
	cpu->flagblock1=reg.edx;

	return 0;

}

/*
 * prints CPU Information (not much)
 * @return void
 */
 void CPU_info(struct cpu_properties *cpu) {
	if (cpu->cpuid_support==true) {
		printf("CPU Manufactory: %s\n",cpu_manufactorys[cpu->manufactory]);
		printf("CPU Architecture: %s\n",architecture[cpu->architecture]);
		printf("CPU Model: %s\n",cpu->cpu_type);
		printf("Static Syscall: Int $0x80\nDynamic Syscall: %s\n",scall[cpu->dsysc]);
		printf("Logical CPUs: %d\n",cpu->logic_cores);
	} else {
		printf("current cpu does not support CPUID\n");
	}
 }


/*
 * Creates structures containing CPU Informations
 * @return void
 */
void INIT_CPUID(void) {
		identify_cpu((struct cpu_properties *)&current_CPU);
		CPU_info((struct cpu_properties *)&current_CPU);
}
