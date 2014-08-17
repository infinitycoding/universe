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
 *  @file /arch/x86/cpuid.c
 *  @brief This file contaions functions for CPU and CPU feature identification.
 *  @todo Standardize these functions for multiple proceccor architectures.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdint.h>
#include <string.h>
#include <printf.h>
#include <cpuid.h>


static char* vendorID[]= 
{
    "AuthenticAMD",	"AMDisbetter!", "GenuineIntel", "CentaurHauls",
    "CyrixInstead", "TransmetaCPU", "GenuineTMx86", "Geode by NSC",
    "NexGenDriven", "RiseRiseRise", "SiS SiS SiS ", "UMC UMC UMC ",
    "VIA VIA VIA ", "Vortex86 SoC"
};  ///@var ID strings of X86 vendors

static char* cpu_manufactorys[]=
{
    "AMD", "AMD", "Intel", "Centaur",
    "Cyrix", "Transmeta", "Transmeta","National Semiconductor",
    "NexGen", "Rise", "SiS", "UMC",
    "VIA", "Vortex", "unknown"
};  ///@var ID strings of X86 manufactories


static char* scall[]= {"Callgate", "Sysenter", "Syscall"};
static char* architecture[]= {"I386","AMD64", "IA32", "IA64"};



struct cpu_properties current_CPU;

/**
 *  @brief Inline function of CPUID
 *  @param cpuid function number
 *  @param output pointer to output register structure
 *  @return sucess (0=sucess, 1=CPUID is not supported)
 */
static void cpuid(uint32_t function,struct cpuid_regs* output)
{
    asm volatile("cpuid":"=a" (output->eax), "=b"(output->ebx),"=c"(output->ecx),"=d"(output->edx):"a"(function));
}


/**
 *  @brief Identify curret CPU
 *  @param pointer to 128byte free bytes for cpu_prpoerties struct
 *  @return sucess (0=sucess, 1=CPUID is not supported)
 */
int identify_cpu(struct cpu_properties *cpu)
{
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
    if (eflags == ref)
        //cpuid is not supported
    {
        cpu->cpuid_support = false;
        return 1;
    }

    //cpuid is supported
    cpu->cpuid_support = true;

    //create struct for processor registers
    struct cpuid_regs reg;
    uint32_t i;

    //call cpuid 0
    cpuid(0, &reg);

    // copy vendor string
    strncpy(cpu->vendor_id    , (char*) &reg.ebx, 4);
    strncpy(cpu->vendor_id + 4, (char*) &reg.edx, 4);
    strncpy(cpu->vendor_id + 8, (char*) &reg.ecx, 4);

    // identify manufactory
    for (i = 0; strncmp(cpu->vendor_id, vendorID[i], 12) && i < 15; i++) {}
    cpu->manufactory = i;

    // save number of basic identify functions
    cpu->max_std_func = reg.eax;

    // get and save maximum numbers of extendet identify functions
    cpuid(0x80000000, &reg);
    cpu->max_spec_func = reg.eax;


    cpuid(1, &reg);
    // get CPU model and Family
    cpu->family     = ((reg.eax & 0x0FF00000) >> 16) | ((reg.eax & 0x00000F00) >> 8);
    cpu->model      = ((reg.eax & 0x000F0000) >> 12) | ((reg.eax & 0x000000F0) >> 4);
    cpu->stepping   =  reg.eax & 0x0000000F;
    cpu->type       = (reg.eax & 0x00003000) >> 12;

    cpu->brandID     = (uint8_t) reg.ebx;
    cpu->clflush     = (uint8_t) (reg.ebx >> 8);
    cpu->logic_cores = (uint8_t) (reg.ebx >> 16);
    cpu->APIC_ID     = (uint8_t) (reg.ebx >> 24);



    cpu->flagblock0 = reg.ecx;
    cpu->flagblock1 = reg.edx;

    if(cpu->max_spec_func > 0x80000000)
    {
        cpuid(0x80000001, &reg);
        if((( reg.edx & (1 << 29) ) >> 29) && cpu->flagblock1 & PAE )
        {
            cpu->LM = true;
        }
    }

    if (cpu->flagblock1 & (1 << 30))
    {
        if(cpu->LM)
            cpu->architecture = IA64;
        else
            cpu->architecture = IA32;
    }
    else
    {
        if(cpu->LM)
            cpu->architecture = AMD64;
        else
            cpu->architecture = I386;
    }
    

    //Detect Dynamic Syscall
    if (cpu->flagblock1 & SEP && cpu->manufactory == 2)
    {
        cpu->dsysc = sysenter;
    }
    else if (cpu->flagblock1 & SEP && cpu->manufactory < 2 && cpu->max_spec_func > 0x80000000)
    {
        cpuid(0x80000001,&reg);
        if (reg.edx & 0x1000)
        {
            cpu->dsysc = syscall;
        }
    }

    //get extended BrandID
    if (cpu->max_spec_func > 0x80000000)
    {
        cpuid(0x80000001, &reg);
        cpu->ext_brandID = (uint16_t)reg.ebx;
    }
    else
    {
        cpu->ext_brandID = 0x3e;
    }


    //Get CPU name/series
    if (cpu->max_spec_func > 0x80000004)   // via Brand String
    {
        for (i=0x80000002; i<=0x80000004; i++)
        {
            cpuid(i,&reg);
            memcpy((cpu->cpu_type+(i-0x80000002)*16),((void*)&reg),16);
        }

    }

    else if(cpu->family == 6 && cpu->model == 6 && cpu->stepping == 3)
    {
        strcat(cpu->cpu_type,"Quemu ");
        strcat(cpu->cpu_type,architecture[cpu->architecture]);
    }

    else
    {
        //standart name
        strcpy(cpu->cpu_type,cpu_manufactorys[cpu->manufactory]);
        strcat(cpu->cpu_type," ");
        strcat(cpu->cpu_type,architecture[cpu->architecture]);
    }

    return 0;
}


/**
 * @brief prints CPU Information (not much)
 * @param cpu CPU properties to print
 * @return void
 */
void CPU_info(struct cpu_properties *cpu)
{
    if (cpu->cpuid_support==true)
    {
        printf("CPU Manufactory: %s\n",cpu_manufactorys[cpu->manufactory]);
        printf("Architecture: %s\n",architecture[cpu->architecture]);
        printf("CPU Model: %s\n",cpu->cpu_type);
        printf("Family: %d  Model: %d  Stepping: %d\n",cpu->family,cpu->model,cpu->stepping);
        printf("Logical CPUs: %d\n",cpu->logic_cores);
        printf("FB0: %#08x  FB1: %#08x\n",cpu->flagblock0, cpu->flagblock1);
        printf("Dynamic Syscall: %s\n",scall[cpu->dsysc]);

        if(cpu->flagblock1 & CLFLSH)
        {
            printf("clflush size: %dB\n",cpu->clflush*8);
        }
        if(cpu->flagblock1 & ACPI)
        {
            printf("APIC ID: %d\n", cpu->APIC_ID);
        }

    }

    else

    {
        printf("current cpu does not support CPUID\n");
    }
}


/**
 * @brief Creates structures containing CPU Informations
 * @return void
 */
void INIT_CPUID(void)
{
    identify_cpu((struct cpu_properties *)&current_CPU);
    CPU_info((struct cpu_properties *)&current_CPU);
}
