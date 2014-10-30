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
 *  @file /arch/x86/dump.c
 *  @todo dump.c has to be rewritten.
 *  @brief Dump tool functions for kernel debugging.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdint.h>
#include <printf.h>
#include <mm/paging.h>
#include <cpu.h>
#include <panic.h>


/**
 *  @brief Dups the current CPU state.
 *  @param cpu The state to be dumped.
 *  @param str String buffer to dump in.
 *  @return Number of written characters.
 */
int cpu_dump(struct cpu_state *cpu, char *str)
{
    int len = 0;

    len += sprintf(str + len, "EAX:  %#010X    EBX:     %#010X\n",  cpu->eax,   cpu->ebx);
    len += sprintf(str + len, "ECX:  %#010X    EDX:     %#010X\n",  cpu->ecx,   cpu->edx);
    len += sprintf(str + len, "ESI:  %#010X    EDI:     %#010X\n",  cpu->esi,   cpu->edi);
    len += sprintf(str + len, "ESP:  %#010X    EBP:     %#010X\n",  cpu->esp,   cpu->ebp);
    len += sprintf(str + len, "CS:   %#010X    DS:      %#010X\n",  cpu->cs,    cpu->ds);
    len += sprintf(str + len, "SS:   %#010X    ES:      %#010X\n",  cpu->ss,    cpu->es);
    len += sprintf(str + len, "GS:   %#010X    FS:      %#010X\n",  cpu->gs,    cpu->fs);
    len += sprintf(str + len, "EIP:  %#010X    EFLAGS:  %#010X\n",  cpu->eip,   cpu->eflags);
    len += sprintf(str + len, "ERRORCODE:  %#010X\n",  cpu->error);

    return len;
}


/**
 *  @brief Prints panic screen for CPU exceptions.
 *  just used in case of untreated exceptions.
 *  @param cpu The cpu state given by the exception.
 */
void exc_panic(struct cpu_state* cpu)
{
    char message[512];
    char *exception = exception_messages[cpu->intr];
    int len = 0;

    len = sprintf(message, "%s\n\n", exception);
    cpu_dump(cpu, message + len);

    panic(message);
}

/**
 * @brief Mapping Dump
 * Only works wehne every PD is mapped in in kernelspace Area
 * @param pointer to the phys. pd frame
 * @return void
 */
void pd_dump(arch_vmm_context_t *context)
{
    int pdi, pti, phys_base, virt_base, size, sec = 0;
    uint32_t *pt;
    uint32_t *pd = context->entries;
    for( pdi = 0; pdi < PD_LENGTH; pdi++ )
    {
        if(sec && (pd[pdi] & 1) == 0)
        {
            printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
            sec = 0;
        }

        while( (pd[pdi] & 1) == 0 )
        {
            pdi++;
        }

        pt = pt_get(context, pdi);

        for( pti = 0; pti < PT_LENGTH; pti++ )
        {
            if(sec && (pt[pti] & 1) == 0)
            {
                printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
                sec = 0;
            }

            while( (pt[pti] & 1) == 0 )
            {
                pti++;
            }
            if(sec == 0)
            {
                sec = 1;
                phys_base = ((pt[pti] | 0xfff) ^ 0xfff);
                virt_base = (pdi << 22) | (pti << 12);
                size = 0;

            }
            else
            {
                if( (phys_base + (4096 * (size+1) )) == ( (pt[pti] | 0xfff) ^ 0xfff) )
                {
                    size++;
                }
                else
                {
                    printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
                    phys_base = ((pt[pti] | 0xfff) ^ 0xfff);
                    virt_base = (pdi << 22) | (pti << 12);
                    size = 0;
                }

            }
        }

    }

    if(sec)
    {
        printf("%#010X - %#010X => %#010X - %#010X\n", phys_base, phys_base + (4096 * size), virt_base, virt_base + (4096 * size));
    }
}



void pd_dump2(arch_vmm_context_t *context)
{
#define END 	if(size > 0) { \
			if(size == 1) { \
				printf("%#010X => %#010X\n", virt_begin, phys_begin); \
			} else { \
				printf("%#010X - %#010X => %#010X - %#010X\n", virt_begin, virt_begin + (size-1)*4096, phys_begin, phys_begin + (size-1)*4096);\
			}\
			size = 0;\
		}

#define START 	virt_begin = (pd_index * 1024 + pt_index) * 4096; \
		phys_begin = paddr; \
		size = 1;

    int pd_index = 0;
    int pt_index = 0;

    uint32_t *pd = context->entries;
    uint32_t *pt = NULL;

    uint32_t virt_begin = 0;
    uint32_t phys_begin = 0;
    int size = 0;

    uint32_t entry = 0;
    uint32_t paddr = 0;

    for(pd_index = 0; pd_index < 1024; pd_index++)
    {
        if(pd[pd_index] & VMM_PRESENT)
        {
            pt = pt_get(context, pd_index);

            for(pt_index = 0; pt_index < 1024; pt_index++)
            {
                entry = pt[pt_index];
                if(entry & VMM_PRESENT)
                {
                    paddr = pt[pt_index] & ~0xfff;

                    if(size == 0)
                    {
                        START
                    }
                    else
                    {
                        if(paddr != phys_begin + size*4096)
                        {
                            END
                        }
                        else
                        {
                            size++;
                        }
                    }
                }
                else
                {
                    END
                }
            }
        }
        else
        {
            END
        }
    }
}


