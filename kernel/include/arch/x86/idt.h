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
#include <pic.h>



//iinterrupt service routines, defined in isr.asm
extern struct cpu_state *isr_0(struct cpu_state *cpu);
extern struct cpu_state *isr_1(struct cpu_state *cpu);
extern struct cpu_state *isr_2(struct cpu_state *cpu);
extern struct cpu_state *isr_3(struct cpu_state *cpu);
extern struct cpu_state *isr_4(struct cpu_state *cpu);
extern struct cpu_state *isr_5(struct cpu_state *cpu);
extern struct cpu_state *isr_6(struct cpu_state *cpu);
extern struct cpu_state *isr_7(struct cpu_state *cpu);
extern struct cpu_state *isr_8(struct cpu_state *cpu);
extern struct cpu_state *isr_9(struct cpu_state *cpu);
extern struct cpu_state *isr_10(struct cpu_state *cpu);
extern struct cpu_state *isr_11(struct cpu_state *cpu);
extern struct cpu_state *isr_12(struct cpu_state *cpu);
extern struct cpu_state *isr_13(struct cpu_state *cpu);
extern struct cpu_state *isr_14(struct cpu_state *cpu);
extern struct cpu_state *isr_15(struct cpu_state *cpu);
extern struct cpu_state *isr_16(struct cpu_state *cpu);
extern struct cpu_state *isr_17(struct cpu_state *cpu);
extern struct cpu_state *isr_18(struct cpu_state *cpu);
extern struct cpu_state *isr_19(struct cpu_state *cpu);
extern struct cpu_state *isr_20(struct cpu_state *cpu);
extern struct cpu_state *isr_21(struct cpu_state *cpu);
extern struct cpu_state *isr_22(struct cpu_state *cpu);
extern struct cpu_state *isr_23(struct cpu_state *cpu);
extern struct cpu_state *isr_24(struct cpu_state *cpu);
extern struct cpu_state *isr_25(struct cpu_state *cpu);
extern struct cpu_state *isr_26(struct cpu_state *cpu);
extern struct cpu_state *isr_27(struct cpu_state *cpu);
extern struct cpu_state *isr_28(struct cpu_state *cpu);
extern struct cpu_state *isr_29(struct cpu_state *cpu);
extern struct cpu_state *isr_30(struct cpu_state *cpu);
extern struct cpu_state *isr_31(struct cpu_state *cpu);
extern struct cpu_state *isr_32(struct cpu_state *cpu);
extern struct cpu_state *isr_33(struct cpu_state *cpu);
extern struct cpu_state *isr_34(struct cpu_state *cpu);
extern struct cpu_state *isr_34(struct cpu_state *cpu);
extern struct cpu_state *isr_35(struct cpu_state *cpu);
extern struct cpu_state *isr_36(struct cpu_state *cpu);
extern struct cpu_state *isr_37(struct cpu_state *cpu);
extern struct cpu_state *isr_38(struct cpu_state *cpu);
extern struct cpu_state *isr_39(struct cpu_state *cpu);
extern struct cpu_state *isr_40(struct cpu_state *cpu);
extern struct cpu_state *isr_41(struct cpu_state *cpu);
extern struct cpu_state *isr_42(struct cpu_state *cpu);
extern struct cpu_state *isr_43(struct cpu_state *cpu);
extern struct cpu_state *isr_44(struct cpu_state *cpu);
extern struct cpu_state *isr_45(struct cpu_state *cpu);
extern struct cpu_state *isr_46(struct cpu_state *cpu);
extern struct cpu_state *isr_47(struct cpu_state *cpu);


extern struct cpu_state *isr_112(struct cpu_state *cpu);
extern struct cpu_state *isr_128(struct cpu_state *cpu);
extern struct cpu_state *isr_default(struct cpu_state *cpu);

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



void lidt(irqnum_t irq);
void Set_IDT_Entry(irqnum_t i,struct cpu_state *(*isr)(struct cpu_state *cpu), uint16_t selector, uint8_t type,privilege_t dpl);

struct cpu_state* irq_handler(struct cpu_state* cpu);

void remap_pic(void);
void INIT_IDT(void);

#endif
