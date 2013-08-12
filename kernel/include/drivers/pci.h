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


#define PCI_CONFIG_DATA    0x0CFC
#define PCI_CONFIG_ADDRESS 0x0CF8

uint8_t pci_config_readb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
uint16_t pci_config_readw(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
uint32_t pci_config_readl(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset);
void pci_config_writeb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint8_t value);
void pci_config_writew(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint16_t value);
void pci_config_writel(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint32_t value);
bool pci_dev_exist(uint8_t bus, uint8_t dev);

void INIT_PCI();

struct pci_dev
{
    uint8_t bus;
    uint8_t dev;
    uint16_t device_ID;
    uint16_t vendor_ID;
    uint8_t reversion_ID;
    uint8_t base_class;
    uint8_t sub_class;
    uint8_t programming_interface;
    bool multifunc;
};

#endif
