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
#include <io.h>
#include <cpu.h>
#include <panic.h>
#include <printf.h>
#include <syscall.h>
#include <sched/scheduler.h>
#include <drivers/pci.h>
#include <drivers/clock.h>


static struct IDT_Entry IDT[256]; ///@var define the interrupt decriptor table
static struct idtpt idtp;


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
void lidt(uint16_t irq)
{
    idtp.limit = (8 * irq)-1;
    idtp.base = IDT;
    asm volatile("lidt %0" : : "m" (idtp));
}


/**
 *  @brief Sends End of Interrupt Signal to the PIC.
 *  @param irq number of the runnig interrupt
 */
void EOI(int irq)
{
    outb(0x20, 0x20);
    if (irq >= 8)
    {
        outb(0xA0, 0x20);
    }
}


/**
 *  @brief Blocks hardware IRQs.
 *  @param mask Number of the IRQ which should be blocked
 */
void pic_mask_irqs(uint16_t mask)
{
    outb(0x21, (uint8_t) mask);
    outb(0xA1, (uint8_t) mask >> 8);
}


/**
 *  @brief Sets an Interrupt Descriptor.
 *  @param i number of the interrupt
 *  @param offset code segment selector of the ISR
 *  @param selector Baseadress of the ISR function
 *  @param type Interrupt type
 *  @param dpl Descriptor previleg level (kernelmode usermode)
 */
void Set_IDT_Entry(int i, uint32_t offset, uint16_t selector, uint8_t type, int dpl)
{
    struct IDT_Entry ent;

    ent.offset_low = offset&0x0000ffff;
    ent.selector = selector;
    ent.ist = 0;
    ent.reserved = 0;
    ent.type = type;
    ent.d = 1;
    ent.dpl = dpl&3;
    ent.present = 1;
    ent.offset_high = (offset&0xffff0000)>>16;

    IDT[i] = ent;
}




/**
 *  @brief General IRQ Handler
 *  @param pointer to cpu_state struct of the interrupted Process
 *  @return pointer to cpu_state struct of the interrupted Process
 */
struct cpu_state* irq_handler(struct cpu_state* cpu)
{
    intr_t irqnum = cpu->intr;
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
 *  @brief Remaps IRQs to irqnum 31.
 */
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
 * @brief Initializes IDT wit exceptions and basical ISRs.
 */
void INIT_IDT(void)
{
    //remap IRQs
    remap_pic();

    //Exceptions
    int i;
    for(i = 0; i < 256; i++)
    {
        Set_IDT_Entry(i, (uint32_t)&isr_default,0x08,INTERRUPT_GATE, 0  );
    }

    Set_IDT_Entry(0,(uint32_t)isr_0,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(1,(uint32_t)isr_1,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(2,(uint32_t)isr_2,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(3,(uint32_t)isr_3,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(4,(uint32_t)isr_4,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(5,(uint32_t)isr_5,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(6,(uint32_t)isr_6,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(7,(uint32_t)isr_7,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(8,(uint32_t)isr_8,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(9,(uint32_t)isr_9,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(10,(uint32_t)isr_10,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(11,(uint32_t)isr_11,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(12,(uint32_t)isr_12,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(13,(uint32_t)isr_13,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(14,(uint32_t)isr_14,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(15,(uint32_t)isr_15,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(16,(uint32_t)isr_16,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(17,(uint32_t)isr_17,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(18,(uint32_t)isr_18,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(19,(uint32_t)isr_19,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(20,(uint32_t)isr_20,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(21,(uint32_t)isr_21,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(22,(uint32_t)isr_22,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(23,(uint32_t)isr_23,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(24,(uint32_t)isr_24,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(25,(uint32_t)isr_25,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(26,(uint32_t)isr_26,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(27,(uint32_t)isr_27,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(28,(uint32_t)isr_28,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(29,(uint32_t)isr_29,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(30,(uint32_t)isr_30,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(31,(uint32_t)isr_31,0x08,INTERRUPT_GATE, 0  );

    //IRQs
    Set_IDT_Entry(32,(uint32_t)isr_32,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(33,(uint32_t)isr_33,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(34,(uint32_t)isr_34,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(35,(uint32_t)isr_35,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(36,(uint32_t)isr_36,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(37,(uint32_t)isr_37,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(38,(uint32_t)isr_38,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(39,(uint32_t)isr_39,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(40,(uint32_t)isr_40,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(41,(uint32_t)isr_41,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(42,(uint32_t)isr_42,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(43,(uint32_t)isr_43,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(44,(uint32_t)isr_44,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(45,(uint32_t)isr_45,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(46,(uint32_t)isr_46,0x08,INTERRUPT_GATE, 0  );
    Set_IDT_Entry(47,(uint32_t)isr_47,0x08,INTERRUPT_GATE, 0  );

    //Syscalls
    Set_IDT_Entry(0x70,(uint32_t)isr_112,0x08,INTERRUPT_GATE, 3  ); //Universe syscall interface
    Set_IDT_Entry(0x80,(uint32_t)isr_128,0x08,INTERRUPT_GATE, 3  ); //Linux syscall interface

    lidt(0xFF);
}
