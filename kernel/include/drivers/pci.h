/*
	Copyright 2012-13 universe coding group (UCG) all rights reserved
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
*/

#ifndef _pic_h_
#define _pci_h_

#include <stdint.h>
#include <io.h>
#include <list.h>


#define PCI_CONFIG_DATA    0x0CFC
#define PCI_CONFIG_ADDRESS 0x0CF8

#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0x02
#define PCI_COMMAND     0x04
#define PCI_STATUS      0x06
#define PCI_REVISION    0x08
#define PCI_CLASS       0x0B
#define PCI_SUBCLASS    0x0A
#define PCI_INTERFACE   0x09
#define PCI_HEADERTYPE  0x0E
#define PCI_BASE        0x10
#define PCI_INTERRUPT   0x3C

#define INTA 1
#define INTB 2
#define INTC 3
#define INTD 4

#define PCI_STANDART_HEADER  0
#define PCI_PCI_BRIDGE_HEADER 1
#define PCI_CARDBUS_BRIDGE_HEADER 2




inline uint8_t pci_readb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
inline uint16_t pci_readw(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
inline uint32_t pci_readl(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
inline void pci_writeb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint8_t value);
inline void pci_writew(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint16_t value);
inline void pci_writel(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint32_t value);
pci_dev_exist(uint8_t bus, uint8_t dev, uint8_t func);

typedef enum
{
    MEMORY = 0,
    PORT = 1,
}pci_base_t;


struct pci_dev_base
{
    uint32_t adress;
    pci_base_t type;
    uint8_t resb;
};



void INIT_PCI();

struct pci_dev
{
    uint8_t bus;
    uint8_t dev;
    uint8_t func;
    uint16_t device_ID;
    uint16_t vendor_ID;
    uint8_t reversion_ID;
    uint8_t base_class;
    uint8_t sub_class;
    uint8_t programming_interface;
    bool multifunc;
    uint8_t header_type;
    uint8_t irq_num;
    uint8_t irq_pin;
    struct pci_dev_base base_adress[6];
};

#ifndef _PCI_C_
    extern list_t *pci_dev_list;
#endif

#endif
