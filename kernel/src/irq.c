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
**/

/**
  @author Simon Diepold aka. Tdotu (Universe Team) <simon.diepold@infinitycoding.de>
*/


#include <idt.h>
#include <io.h>
#include <cpu.h>
#include <panic.h>
#include <printf.h>
#include <syscall.h>
#include <scheduler.h>
#include <drivers/pci.h>

static struct IDT_Entry IDT[256];
static struct idtpt idtp;


/**
 * Load IDT
 * @param nuber of the last defined Descriptor
 * @return void
 **/
void lidt(uint16_t irq)
{
	idtp.limit = (8 * irq)-1;
	idtp.base = IDT;
	asm volatile("lidt %0" : : "m" (idtp));
}


/**
 * Send End of Interrupt Signal to the PIC
 * @param number of the runnig interrupt
 * @return void
 **/
void EOI(int irq)
{
	outb(0x20, 0x20);
	if (irq >= 8) {
		outb(0xA0, 0x20);
	}
}


/**
 * block hardware IRQs
 * @param number of the IRQ which should be blocked
 * @return void
 **/
void pic_mask_irqs(uint16_t mask)
{
   outb(0x21, (uint8_t) mask);
   outb(0xA1, (uint8_t) mask >> 8);
}


/**
 * Set an Interrupt Descriptor
 * @param 0 number of the interrupt
 * @param 1 code segment selector of the ISR
 * @param 2 Baseadress of the ISR function
 * @param 3 Flags
 * @return void
 **/
void Set_IDT_Entry(uint32_t intnr, uint16_t selector,uint32_t Base, uint16_t flags)
{
	IDT[intnr].Base_low = (uint16_t)Base;
	IDT[intnr].selector = selector;
	IDT[intnr].flags = flags;
	IDT[intnr].Base_hi = (uint16_t)(Base>>16);
}


 //pointer arrays for exception and interrupt handlers

static void (*irq[16])(struct cpu_state **cpu) = {
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL
};

static void (*exc[32])(struct cpu_state **cpu) = {
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL,
	NULL, NULL
};



/**
 * install an interrupthandler
 * @param 0 interrupt number
 * @param 1 pointer to the handler function
 * @return false  -> Handler is already seted-up
 * @return true -> Handler sucessfully installed
 **/
int install_irq(int intr,void *handler)
{
	if (irq[intr] != NULL) {
	    return false;
	}

	irq[intr] = handler;
	return true;
}


/**
 * install an exception handler
 * @param 0 exception number
 * @param 1 pointer to the handler function
 * @return true  -> Handler is already seted-up
 * @return false -> Handler sucessfully installed
 **/
int install_exc(int excnum, void *handler)
{
	if (exc[excnum] != NULL) {
	    return false;
	}

	exc[excnum] = handler;
	return true;
}

/**
 * deinstall IRQ handlder
 * @param interrupt number
 * @return void
**/
void deinstall_irq(int intr)
{
	irq[intr] = NULL;
}


/**
 * deinstall exception handlder
 * @param exception number
 * @return void
**/
void deinstall_exc(int excnum)
{
	exc[excnum] = NULL;
}


/**
 * General IRQ Handler
 * @param pointer to cpu_state struct of the interrupted Process
 * @return pointer to cpu_state struct of the interrupted Process
 **/
struct cpu_state* irq_handler(struct cpu_state* cpu)
{
	//Exceptions
	if (cpu->intr < 32)
	{
		if (exc[cpu->intr] != NULL)
		{
			exc[cpu->intr](&cpu);
			return cpu;
		}
		else
		{
			exc_panic(cpu);
		}
	}
	//Taskscheduler
	else if(cpu->intr == 32)
	{
        cpu = task_schedule(cpu);
	}
	//IRQs
	else if (cpu->intr < 46)
	{
	    int irqnum = cpu->intr - 32;
		if (irq[irqnum] != NULL)
		{
			irq[irqnum](&cpu);
		}
		EOI(irqnum);
		return cpu;
	}
	//universe syscall
	else if(cpu->intr == 0x70)
	{
        universe_syscall_handler(&cpu);
	}
	//linux syscall
	else if(cpu->intr == 0x80)
	{
        linux_syscall_handler(&cpu);
	}
	//pci IRQ
	else if(cpu->intr == 50)
	{
        pci_irq_handler();
	}
	//unspecified ISRs
	else
	{
        panic("A unspecified ISR was called.");
	}
	return cpu;
}

/**
 * remap IRQs to irqnum 31
 * @param void
 * @return void
 **/
void remap_pic(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 32);
	outb(0xA1, 32+8);
	outb(0x21, 0x04);
	outb(0xA1, 2);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
}

/**
 * Initialize IDT wit exceptions and basical ISRs
 * @param 0 void
 * @param 1 void
 **/
void INIT_IDT(void)
{
	remap_pic();
	//Exceptions
	int i;
	for(i = 0; i < 256; i++)
	{
        Set_IDT_Entry(i,0x8,(uint32_t)isr_default,0xEE00);
	}

	Set_IDT_Entry(0,0x8,(uint32_t)isr_0,0xEE00); Set_IDT_Entry(1,0x8,(uint32_t)isr_1,0xEE00); Set_IDT_Entry(2,0x8,(uint32_t)isr_2,0xEE00);
	Set_IDT_Entry(3,0x8,(uint32_t)isr_3,0xEE00); Set_IDT_Entry(4,0x8,(uint32_t)isr_4,0xEE00); Set_IDT_Entry(5,0x8,(uint32_t)isr_5,0xEE00);
	Set_IDT_Entry(6,0x8,(uint32_t)isr_6,0xEE00); Set_IDT_Entry(7,0x8,(uint32_t)isr_7,0xEE00); Set_IDT_Entry(8,0x8,(uint32_t)isr_8,0xEE00);
	Set_IDT_Entry(9,0x8,(uint32_t)isr_9,0xEE00); Set_IDT_Entry(10,0x8,(uint32_t)isr_10,0xEE00); Set_IDT_Entry(11,0x8,(uint32_t)isr_11,0xEE00);
	Set_IDT_Entry(12,0x8,(uint32_t)isr_12,0xEE00); Set_IDT_Entry(13,0x8,(uint32_t)isr_13,0xEE00); Set_IDT_Entry(14,0x8,(uint32_t)isr_14,0xEE00);
	Set_IDT_Entry(15,0x8,(uint32_t)isr_15,0xEE00); Set_IDT_Entry(16,0x8,(uint32_t)isr_16,0xEE00); Set_IDT_Entry(17,0x8,(uint32_t)isr_17,0xEE00);
	Set_IDT_Entry(18,0x8,(uint32_t)isr_18,0xEE00); Set_IDT_Entry(19,0x8,(uint32_t)isr_19,0xEE00); Set_IDT_Entry(20,0x8,(uint32_t)isr_20,0xEE00);
	Set_IDT_Entry(21,0x8,(uint32_t)isr_21,0xEE00); Set_IDT_Entry(22,0x8,(uint32_t)isr_22,0xEE00); Set_IDT_Entry(23,0x8,(uint32_t)isr_23,0xEE00);
	Set_IDT_Entry(24,0x8,(uint32_t)isr_24,0xEE00); Set_IDT_Entry(25,0x8,(uint32_t)isr_25,0xEE00); Set_IDT_Entry(26,0x8,(uint32_t)isr_26,0xEE00);
	Set_IDT_Entry(27,0x8,(uint32_t)isr_27,0xEE00); Set_IDT_Entry(28,0x8,(uint32_t)isr_28,0xEE00); Set_IDT_Entry(29,0x8,(uint32_t)isr_29,0xEE00);
	Set_IDT_Entry(30,0x8,(uint32_t)isr_30,0xEE00); Set_IDT_Entry(31,0x8,(uint32_t)isr_31,0xEE00);
	//IRQs
	Set_IDT_Entry(32,0x8,(uint32_t)isr_32,0xEE00); Set_IDT_Entry(33,0x8,(uint32_t)isr_33,0xEE00);
	Set_IDT_Entry(34,0x8,(uint32_t)isr_34,0xEE00); Set_IDT_Entry(35,0x8,(uint32_t)isr_35,0xEE00);
	Set_IDT_Entry(36,0x8,(uint32_t)isr_36,0xEE00); Set_IDT_Entry(37,0x8,(uint32_t)isr_37,0xEE00);
	Set_IDT_Entry(38,0x8,(uint32_t)isr_38,0xEE00); Set_IDT_Entry(39,0x8,(uint32_t)isr_39,0xEE00);
	Set_IDT_Entry(40,0x8,(uint32_t)isr_40,0xEE00); Set_IDT_Entry(41,0x8,(uint32_t)isr_41,0xEE00);
	Set_IDT_Entry(42,0x8,(uint32_t)isr_42,0xEE00); Set_IDT_Entry(43,0x8,(uint32_t)isr_43,0xEE00);
	Set_IDT_Entry(44,0x8,(uint32_t)isr_44,0xEE00); Set_IDT_Entry(45,0x8,(uint32_t)isr_45,0xEE00);
	Set_IDT_Entry(46,0x8,(uint32_t)isr_46,0xEE00); Set_IDT_Entry(47,0x8,(uint32_t)isr_47,0xEE00);

	Set_IDT_Entry(0x70,0x8,(uint32_t)isr_112,0xEE00); //Universe syscall interface
	Set_IDT_Entry(0x80,0x8,(uint32_t)isr_128,0xEE00); //Linux syscall interface
	lidt(129);
}
