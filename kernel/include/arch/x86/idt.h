#ifndef _IDT_H_
#define _IDT_H_

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
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <stdint.h>
#include <cpu.h>

/*
	TODO: Define INTs here
*/

enum INT_NUM
{
    INT_PAGE_FAULT = 0x0E
};

#define IRQ_OFFSET 32

extern void isr_0(void);
extern void isr_1(void);
extern void isr_2(void);
extern void isr_3(void);
extern void isr_4(void);
extern void isr_5(void);
extern void isr_6(void);
extern void isr_7(void);
extern void isr_8(void);
extern void isr_9(void);
extern void isr_10(void);
extern void isr_11(void);
extern void isr_12(void);
extern void isr_13(void);
extern void isr_14(void);
extern void isr_15(void);
extern void isr_16(void);
extern void isr_17(void);
extern void isr_18(void);
extern void isr_19(void);
extern void isr_20(void);
extern void isr_21(void);
extern void isr_22(void);
extern void isr_23(void);
extern void isr_24(void);
extern void isr_25(void);
extern void isr_26(void);
extern void isr_27(void);
extern void isr_28(void);
extern void isr_29(void);
extern void isr_30(void);
extern void isr_31(void);
extern void isr_32(void);
extern void isr_33(void);
extern void isr_34(void);
extern void isr_35(void);
extern void isr_36(void);
extern void isr_37(void);
extern void isr_38(void);
extern void isr_39(void);
extern void isr_40(void);
extern void isr_41(void);
extern void isr_42(void);
extern void isr_43(void);
extern void isr_44(void);
extern void isr_45(void);
extern void isr_46(void);
extern void isr_47(void);

extern void isr_112(void);
extern void isr_128(void);
extern void isr_default(void);
extern void isr_pci(void);

#define INTERRUPT_GATE 0x06
#define TRAP_GATE 0x07
#define TASK_GATE 0x05

struct IDT_Entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist :3;
    uint8_t reserved :5;
    uint8_t type :3;
    uint8_t d :2;
    uint8_t dpl :2;
    uint8_t present :1;
    uint16_t offset_high;
} __attribute__((packed));

struct idtpt
{
    uint16_t limit;
    void* base;
} __attribute__((packed));

typedef uint32_t intr_t;


void lidt(uint16_t irq);
void EOI(int irq);
void Set_IDT_Entry(int i, uint32_t offset, uint16_t selector, uint8_t type, int dpl);

struct cpu_state* irq_handler(struct cpu_state* cpu);

void remap_pic(void);
void INIT_IDT(void);

#endif
