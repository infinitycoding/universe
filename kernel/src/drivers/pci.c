/*
	Copyright 2012-2013 universe coding group (UCG) all rights reserved
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

#include <drivers/pci.h>
#include <printf.h>
#include <list.h>
#include <heap.h>


/**
 * reads a byte from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
uint8_t pci_config_readb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    return inb(PCI_CONFIG_DATA);
}

/**
 * reads a word from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
uint16_t pci_config_readw(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    return inw(PCI_CONFIG_DATA);
}


/**
 * reads a double word from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
uint32_t pci_config_readl(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    return inl(PCI_CONFIG_DATA);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
void pci_config_writeb(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint8_t value)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    outb(PCI_CONFIG_DATA,value);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
void pci_config_writew(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint16_t value)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    outw(PCI_CONFIG_DATA,value);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
void pci_config_writed(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset, uint32_t value)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000|(bus<<16)|(dev<<11)|(func<<8)|(offset&0xFC));
    outl(PCI_CONFIG_DATA,value);
}


/**
 * checks if PCI device exists
 *  @param Bus
 *  @param Device/Slot
 *  @return true (device exists) or false (device does not exist)
 */
bool pci_dev_exist(uint8_t bus, uint8_t dev)
{
    uint16_t vendor_ID = pci_config_readw(bus,dev,0,0);
    if(vendor_ID == 0 || vendor_ID == 0xFFFF)
        return false;
    return true;
}

char *pci_dev_names[][16]={
    {"non vga device", "vga device"}, {"SCSI", "IDE", "Floppy", "IPI", "RAID", "ATA-controler", "SATA-controler","SAS"},
    {"Ethernet", "Token Ring", "FDDI", "ATM", "ISDN", "FIP", "PICMG 2.14"}, {"VGA", "XGA", "3D"}, {"Video", "Audio", "Phone", "HD-Audio"},
    {"RAM", "Flash"},{"Host", "ISA", "EISA-Bridge", "MicroChannel", "PCI-Bridge", "PCMCIA-Bridge", "NuBus-Bridge", "CardBus-Bridge", "Raceway", "Semitransparente PCI/PCI-Bridge","InfinityBand"}
};


//Linked list of PCI devices
list_t *pci_dev_list;


void INIT_PCI()
{
    pci_dev_list = list_create();
    int dev,bus = 0;

    for(bus = 0; bus < 8; bus++)
    {

        for(dev = 0; dev < 32; dev++)
        {
            int func;

            if(pci_dev_exist(bus, dev))
            {
                uint32_t classcode = pci_config_readl(bus,dev,0,8);

                struct pci_dev *current_dev = malloc(sizeof(struct pci_dev));
                current_dev->bus = bus;
                current_dev->dev = dev;
                current_dev->device_ID = pci_config_readl(bus,dev,func,0)>>16;
                current_dev->vendor_ID = pci_config_readw(bus,dev,func,0);

                current_dev->reversion_ID = (uint8_t)classcode;
                current_dev->programming_interface = (uint8_t) (classcode >> 8);
                current_dev->sub_class = (uint8_t) (classcode >> 16);
                current_dev->base_class = (uint8_t) (classcode >> 24);

                list_push_front(pci_dev_list,current_dev);

                if(((uint8_t) (classcode >> 24))<8 && ((uint8_t) (classcode >> 16)) < 16)
                    printf("slot:%d  vendor: %#04x  device: %0#4x  %s\n",dev,pci_config_readw(bus,dev,0,0),pci_config_readl(bus,dev,0,0)>>16,pci_dev_names[(uint8_t) (classcode >> 24)][(uint8_t) (classcode >> 16)]);
                else
                    printf("slot:%d  vendor: %#04x  device: %0#4x  %s\n",dev,pci_config_readw(bus,dev,0,0),pci_config_readl(bus,dev,0,0)>>16,"unknown");

            }

        }
    }

}
