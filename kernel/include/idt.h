#ifndef	_idt_h_
#define	_idt_h_
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

	#include <stdint.h>
	#include <cpu.h>

	extern void int_0(void);
	extern void int_1(void);
	extern void int_2(void);
	extern void int_3(void);
	extern void int_4(void);
	extern void int_5(void);
	extern void int_6(void);
	extern void int_7(void);
	extern void int_8(void);
	extern void int_9(void);
	extern void int_10(void);
	extern void int_11(void);
	extern void int_12(void);
	extern void int_13(void);
	extern void int_14(void);
	extern void int_15(void);
	extern void int_16(void);
	extern void int_17(void);
	extern void int_18(void);
	extern void int_19(void);
	extern void int_20(void);
	extern void int_21(void);
	extern void int_22(void);
	extern void int_23(void);
	extern void int_24(void);
	extern void int_25(void);
	extern void int_26(void);
	extern void int_27(void);
	extern void int_28(void);
	extern void int_29(void);
	extern void int_30(void);
	extern void int_31(void);

	extern void irq_0(void);
	extern void irq_1(void);
	extern void irq_2(void);
	extern void irq_3(void);
	extern void irq_4(void);
	extern void irq_5(void);
	extern void irq_6(void);
	extern void irq_7(void);
	extern void irq_8(void);
	extern void irq_9(void);
	extern void irq_10(void);
	extern void irq_11(void);
	extern void irq_12(void);
	extern void irq_13(void);
	extern void irq_14(void);
	extern void irq_15(void);

	struct IDT_Entry{
	uint16_t Base_low;
	uint16_t selector;
	uint16_t flags;
	uint16_t Base_hi;
	}__attribute__((packed));

	struct idtpt{
		uint16_t limit;
		void* base;
	} __attribute__((packed));

	
	void lidt(uint16_t irq);
	void EOI(int irq);
	void Set_IDT_Entry(uint32_t intnr, uint16_t selector,uint32_t Base, uint16_t flags);

	struct cpustate* irq_handler(struct cpustate* cpu);
	int install_irq(int irqnum,void (*handler)(void));
	void deinstall_irq(int irqnum);

	struct cpustate* exception_handler(struct cpustate* cpu);
	int install_exc(int excnum,void (*handler)(void));
	void deinstall_exc(int excnum);

	void remap_pic(void);
	void INIT_IDT(void);

#endif
