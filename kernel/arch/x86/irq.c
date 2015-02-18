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
 *  @file /arch/x86/irq.c
 *  @brief Interrupt and exception handling module.
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <idt.h>
#include <panic.h>
#include <syscall.h>
#include <sched/scheduler.h>
#include <clock.h>


static struct IDT_Entry IDT[256]; ///@var define the interrupt decriptor table


char *exception_messages[] =
{
    "Division By Zero",        "Debug",                         "Non Maskable Interrupt",    "Breakpoint",
    "Into Detected Overflow",  "Out of Bounds",                 "Invalid Opcode",            "No Coprocessor",
    "Double Fault",            "Coprocessor Segment Overrun",   "Bad TSS",                   "Segment Not Present",
    "Stack Fault",             "General Protection Fault",      "Page Fault",                "Unknown Interrupt",
    "Coprocessor Fault",       "Alignment Check",               "Machine Check",             "SIMD Floating Point",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved",
    "Reserved",                "Reserved",                      "Reserved",                  "Reserved"
}; ///@var List of all possible exceptions


/**
 *  @brief Loads IDT.
 *  @param irq nuber of the last defined Descriptor
 */
void lidt(irqnum_t irq)
{
    struct idtpt idtp;
    idtp.limit = (8 * irq)-1;
    idtp.base = IDT;
    asm volatile("lidt %0" : : "m" (idtp));
}


/**
 *  @brief Sets an Interrupt Descriptor.
 *  @param i number of the interrupt
 *  @param isr pointer to the interrupt service routine
 *  @param selector code segment selector of the ISR
 *  @param type Interrupt type
 *  @param dpl Descriptor previleg level (kernelmode usermode)
 */
void Set_IDT_Entry(irqnum_t i,struct cpu_state *(*isr)(struct cpu_state *cpu), uint16_t selector, uint8_t type, privilege_t dpl)
{
    struct IDT_Entry ent;
    ent.offset_low = ((uint32_t)isr)&0x0000ffff;
    ent.selector = selector;
    ent.ist = 0;
    ent.reserved = 0;
    ent.type = type;
    ent.d = 1;
    ent.dpl = dpl&3;
    ent.present = 1;
    ent.offset_high = (((uint32_t)isr)&0xffff0000)>>16;
    IDT[i] = ent;
}




/**
 *  @brief General IRQ Handler
 *  @param pointer to cpu_state struct of the interrupted Process
 *  @return pointer to cpu_state struct of the interrupted Process
 */
struct cpu_state* irq_handler(struct cpu_state* cpu)
{
    irqnum_t irqnum = cpu->intr;
    //Taskscheduler
    if(irqnum == 32)
    {
        sync_sys_clock();
        cpu = task_schedule(cpu);
        EOI(0);
    }
    //Exceptions and IRQs
    else if(irqnum < 46)
    {
        if (!handle_interupts(&cpu))
        {
            if(irqnum == 0xe)
                page_fault_handler(&cpu);
            if(irqnum < 32)
                exc_panic(cpu);
        }

        if(irqnum > 32)
            EOI(irqnum-IRQ_OFFSET);
        else
            EOI(irqnum);
    }
    //universe syscall
    else if(irqnum == 0x70)
        universe_syscall_handler(&cpu);

    //linux syscall
    else if(irqnum == 0x80)
        linux_syscall_handler(&cpu);

    //unspecified ISRs
    else
        panic("A unspecified ISR was called.");

    return cpu;
}



/**
 * @brief Initializes IDT wit exceptions and basical ISRs.
 */
void INIT_IDT(void)
{
    //Exceptions
    int i;
    for(i = 48; i < 256; i++)
    {
        Set_IDT_Entry(i, isr_default, 0x08, INTERRUPT_GATE, KERNELMODE);
    }

    Set_IDT_Entry(0 , isr_0 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(1 , isr_1 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(2 , isr_2 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(3 , isr_3 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(4 , isr_4 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(5 , isr_5 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(6 , isr_6 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(7 , isr_7 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(8 , isr_8 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(9 , isr_9 , 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(10, isr_10, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(11, isr_11, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(12, isr_12, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(13, isr_13, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(14, isr_14, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(15, isr_15, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(16, isr_16, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(17, isr_17, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(18, isr_18, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(19, isr_19, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(20, isr_20, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(21, isr_21, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(22, isr_22, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(23, isr_23, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(24, isr_24, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(25, isr_25, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(26, isr_26, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(27, isr_27, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(28, isr_28, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(29, isr_29, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(30, isr_30, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(31, isr_31, 0x08, INTERRUPT_GATE, KERNELMODE);

    //IRQs
    Set_IDT_Entry(32, isr_32, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(33, isr_33, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(34, isr_34, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(35, isr_35, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(36, isr_36, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(37, isr_37, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(38, isr_38, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(39, isr_39, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(40, isr_40, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(41, isr_41, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(42, isr_42, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(43, isr_43, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(44, isr_44, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(45, isr_45, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(46, isr_46, 0x08, INTERRUPT_GATE, KERNELMODE);
    Set_IDT_Entry(47, isr_47, 0x08, INTERRUPT_GATE, KERNELMODE);

    //Syscalls
    Set_IDT_Entry(0x70, isr_112, 0x08, INTERRUPT_GATE, USERMODE); //Universe syscall interface
    Set_IDT_Entry(0x80, isr_128, 0x08, INTERRUPT_GATE, USERMODE); //Linux syscall interface

    lidt(0xFF);
}
