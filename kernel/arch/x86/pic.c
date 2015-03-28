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
 *  @file /arch/x86/pic.c
 *  @brief Functions for setting up the programmable interrupt controler (pic)
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <arch/pic.h>
#include <io.h>

/**
 *  @brief Sends End of Interrupt Signal to the PIC which
 *  allows the pic to dispatch the next irq.
 *  @param irq number of the runnig interrupt
 */
void EOI(irqnum_t irq)
{
    outb(PIC_MASTER_CMD, PIC_EOI);
    if (irq >= 8)
        outb(PIC_SLAVE_CMD, PIC_EOI);
}


/**
 *  @brief Remaps IRQs to irqnum 31.
 */
void INIT_PIC(void)
{
    outb(PIC_MASTER_CMD  , PIC_INIT		);
    outb(PIC_SLAVE_CMD   , PIC_INIT		);
    outb(PIC_MASTER_DATA , IRQ_OFFSET		);
    outb(PIC_SLAVE_DATA  , IRQ_OFFSET+8		);
    outb(PIC_MASTER_DATA , PIC_SET_CHANNEL	);
    outb(PIC_SLAVE_DATA  , PIC_DEFAULT_CHANNEL	);
    outb(PIC_MASTER_DATA , PIC_X86_MODE		);
    outb(PIC_SLAVE_DATA  , PIC_X86_MODE		);
}
