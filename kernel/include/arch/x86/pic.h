#ifndef _pic_h_
#define _pic_h_

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

#include <stdint.h>

#define PIC_MASTER_CMD	0x20
#define PIC_MASTER_DATA	0x21
#define PIC_SLAVE_CMD	0xA0
#define PIC_SLAVE_DATA	0xA1
#define PIC_EOI 0x20
#define PIC_INIT 0x11

#define IRQ_OFFSET 32
#define PIC_SET_CHANNEL 0x4
#define PIC_DEFAULT_CHANNEL 0x2
#define PIC_X86_MODE 0x1

typedef uint16_t irqnum_t;

void EOI(irqnum_t irq);
void INIT_PIC(void);
#endif