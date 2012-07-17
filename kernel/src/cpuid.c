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

    Total Verbuggt :D
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



struct cpu_properties current_CPU;

/*
 * Creates structures containing CPU Informations
 * @return sucess (0=sucess, 1=CPUID is not supported)
 */
int INIT_CPUID(void){
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
	if(eflags==ref){ //cpuid is not supported
	    current_CPU.cpuid_support=false;
	    return 1;
	}

	struct cpuid_regs reg;
	uint32_t i;
	char vendor[13];
	cpuid(0,&reg);
    strncpy(vendor,(char*)&reg.ebx,4);
    strncpy(vendor+4,(char*)&reg.edx,4);
    strncpy(vendor+8,(char*)&reg.ecx,4);
	for(i=0;strncmp(vendor,vendorID[i],12)&& i<15;i++){}
	current_CPU.manufactor=i;
	current_CPU.max_std_func=reg.eax;
    cpuid(0x80000000,&reg);
	current_CPU.max_spec_func=reg.eax;

	cpuid(1,&reg);
	current_CPU.family = (reg.eax & 0x00000F00)>>8;
	current_CPU.model = (reg.eax & 0x000000F0)>>4;
	current_CPU.stepping = reg.eax & 0x0000000F;
	current_CPU.brandID = (uint8_t)reg.ebx;

	if( (!(reg.edx & 0x800)) && (current_CPU.family == 6) && (current_CPU.model < 3) && (current_CPU.stepping < 3)){
		current_CPU.dsysc=sysenter;
	} else if(current_CPU.max_spec_func>0x80000000){
        cpuid(0x80000001,&reg);
        if(reg.edx& 0x1000){
            current_CPU.dsysc=syscall;
        }
	}

	if(current_CPU.max_spec_func>0x80000000){
        cpuid(0x80000001,&reg);
        current_CPU.ext_brandID=(uint16_t)reg.ebx;
	}else{
        current_CPU.ext_brandID=0x3e;
	}


    //Get CPU name/series
	if(current_CPU.max_spec_func>0x80000004){ // via Brand String
        for(i=0x80000002;i<=0x80000004;i++){
            cpuid(i,&reg);
            memcpy((current_CPU.cpu_type+(i-0x80000002)*16),((void*)&reg),16);
        }
	}else if(current_CPU.brandID){ //via Brand ID
        if(current_CPU.manufactor==2){ //Intel CPU
            strcpy(current_CPU.cpu_type,brandID_Intel[current_CPU.brandID-1]);
        }
	}else if(current_CPU.manufactor<2){ //AMD CPU
        int ID=(current_CPU.ext_brandID>>6)& 0x3ff;
        if(ID>0x3e){ID=0x3e;}
        //uint8_t NN=current_CPU.ext_brandID &0x3F;
        //TODO: replace XX,YY,ZZ,TT,RR,EE
        strcpy(current_CPU.cpu_type,brandID_AMD[ID]);

	}else{ //standart name
        strcpy(current_CPU.cpu_type,cpu_manufactorys[current_CPU.manufactor]);
        strcat(current_CPU.cpu_type," X86 CPU");
	}
    // TODO: identify CPU through family,model,stepping, socket
	// TODO: check and save APIC, SMP infos


	cpuid(1,&reg);


	return 0;
}

/*
 * prints CPU Information
 * @return void
 */

 void CPU_info(void){
    printf("CPU Manufactur: %s\n",cpu_manufactorys[current_CPU.manufactor]);
    printf("Model: %s\n",current_CPU.cpu_type);
    printf("Static Syscall: Int $0x80\nDynamic Syscall: %s\n",scall[current_CPU.dsysc]);
 }


