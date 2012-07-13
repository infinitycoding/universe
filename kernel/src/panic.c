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

    Das Universe Kernel ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Das Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    Universe Kernel wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/

#include <idt.h>
#include <cpu.h>
#include <printf.h>

char* exception_messages[] =
{
    "Division By Zero",        "Debug",                         "Non Maskable Interrupt",    "Breakpoint",
    "Into Detected Overflow",  "Out of Bounds",                 "Invalid Opcode",            "No Coprocessor",
    "Double Fault",            "Coprocessor Segment Overrun",   "Bad TSS",                   "Segment Not Present",
    "Stack Fault",             "General Protection Fault",      "Page Fault",                "Unknown Interrupt",
    "Coprocessor Fault",       "Alignment Check",               "Machine Check",             "SIMD Floating Point",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved"
};


// just used in case of untreated exceptions
void exc_panic(struct cpu_state* cpu){
	int i;
	char* exception=exception_messages[cpu->intr];
	/*for(i=0;i<cpu->intr;i++){
		while(exception[0]){
			exception++;
		}
	}*/
	printf("%s  Errorcode:%-10x\n\n",exception,cpu->error);
	printf("EAX: %-10x  EBX: %-10x\n",cpu->eax,cpu->ebx);
	printf("ECX: %-10x  EDX: %-10x\n",cpu->ecx,cpu->edx);
	printf("ESI: %-10x  EDI: %-10x\n",cpu->esi,cpu->edi);
	printf("ESP: %-10x  EBP: %-10x\n",cpu->esp,cpu->ebp);
	printf("CS: %-10x  DS: %-10x\n",cpu->cs,cpu->ds);
	printf("SS: %-10x  ES: %-10x\n",cpu->ss,cpu->es);
	printf("GS: %-10x  FS: %-10x\n",cpu->gs,cpu->fs);
	printf("EIP: %-10x  EFLAGS: %-10x\n",cpu->eip,cpu->eflags);
	printf("System halted...\n");
	while(1){
		asm volatile("cli; hlt");	
	}
}

//hope this function never will be used ;)
void kernel_panic(char* message){
	printf("%s occured\n",message);
	//Dump System Structures
	printf("System halted...\n");
	while(1){}
}
