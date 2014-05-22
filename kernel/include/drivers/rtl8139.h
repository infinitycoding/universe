#ifndef _RTL8193_H_
#define _RTL8193_H_

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

#include <io.h>

#define VENDOR_ID 0x10EC
#define DEVICE_ID 0x8139


//imported from tyndur
#define REG_ID0 0x00
#define REG_ID4 0x04

#define REG_TRANSMIT_STATUS0 0x10
#define REG_TRANSMIT_ADDR0 0x20
#define REG_RECEIVE_BUFFER 0x30
#define REG_COMMAND 0x37
#define REG_CUR_READ_ADDR 0x38
#define REG_INTERRUPT_MASK 0x3C
#define REG_INTERRUPT_STATUS 0x3E
#define REG_TRANSMIT_CONFIGURATION 0x40
#define REG_RECEIVE_CONFIGURATION 0x44


#define RESET_CARD (1 << 4)
#define ENABLE_RECIEVER (1 << 3)
#define ENABLE_TRANSMITTER (1 << 2)
#define ALL_INTERRUPTS 0xffff

struct rtl_dev
{
    uint32_t portbase;
    uint32_t phys_membase;
    uint32_t virt_membase;
    uint32_t mem_size;
    int recievebuffer;
    int sendbuffer;
    struct pci_dev *dev;
};

void INIT_RTL8139(void);
void rtl8139_isr(struct pci_dev *dev);
void rtl8139_reset(struct rtl_dev * dev);


static inline void rtl_outb(struct rtl_dev *dev, uint32_t offset, uint8_t value)
{
    outb(dev->portbase+offset, value);
}

static inline void rtl_outw(struct rtl_dev *dev, uint32_t offset, uint16_t value)
{
    outw(dev->portbase+offset, value);
}

static inline void rtl_outl(struct rtl_dev *dev, uint32_t offset, uint32_t value)
{
    outl(dev->portbase+offset, value);
}

static inline uint8_t rtl_inb(struct rtl_dev *dev, uint32_t offset)
{
    return inb(dev->portbase+offset);
}

static inline uint16_t rtl_inw(struct rtl_dev *dev, uint32_t offset)
{
    return inw(dev->portbase+offset);
}

static inline uint32_t rtl_inl(struct rtl_dev *dev, uint32_t offset)
{
    return inl(dev->portbase+offset);
}

#endif
