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


#define _PCI_C_

#include <pci.h>
#include <stdio.h>


uint32_t pci_read(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000 | (bus << 16) | (dev << 11) |( func << 8) | (offset & 0xFC));
    return inl(PCI_CONFIG_DATA) >> (8 * (offset % 4));
}



/**
 * reads a byte from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
uint8_t pci_readb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    return pci_read(bus, dev, func, offset) & 0xff;
}

/**
 * reads a word from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
inline uint16_t pci_readw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    return pci_read(bus, dev, func, offset) & 0xffff;
}


/**
 * reads a double word from the PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @return value from the input adress
 */
inline uint32_t pci_readl(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    return pci_read(bus, dev, func, offset);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
inline void pci_writeb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint8_t value)
{
    outl(PCI_CONFIG_ADDRESS,0x80000000 | (bus << 16) | (dev << 11 ) | (func << 8) | (offset & 0xFC));
    outb(PCI_CONFIG_DATA + (offset & 3), value);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
inline void pci_writew(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint16_t value)
{
    outl(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC));
    outw(PCI_CONFIG_DATA + (offset & 2), value);
}


/**
 * writes value into to PCI configuration space
 * @param Bus
 * @param Device
 * @param function
 * @param Offset
 * @param Value
 */
inline void pci_writel(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t value)
{
    outl(PCI_CONFIG_ADDRESS, 0x80000000 | (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC));
    outl(PCI_CONFIG_DATA, value);
}


/**
 * checks if PCI device exists
 *  @param Bus
 *  @param Device/Slot
 *  @return true (device exists) or false (device does not exist)
 */
bool pci_dev_exist(uint8_t bus, uint8_t dev, uint8_t func)
{
    uint16_t vendor_ID = pci_readw(bus, dev, func, 0);
    if(vendor_ID == 0 || vendor_ID == 0xFFFF)
        return false;
    return true;
}

/**
 * Collection of pci device classes
 **/
char *pci_classes[] = {
    "storage", "network", "display", "multimedia", "memory",
    "bridge", "bus/port", "perephiery", "input", "docking station",
    "processor", "serial bus", "wireless", "intelegent", "Satelite",
    "crypto", "signal processor"
};


/**
 * Collection of pci device names orderd by class and subclass pci_dev_names[class][subclass]
 **/
char *pci_dev_names[][16]={
    {"non vga device", "vga device"}, {"SCSI", "IDE", "Floppy", "IPI", "RAID", "ATA-controler", "SATA-controler","SAS"},
    {"Ethernet", "Token Ring", "FDDI", "ATM", "ISDN", "FIP", "PICMG 2.14"}, {"VGA", "XGA", "3D"}, {"Video", "Audio", "Phone", "HD-Audio"},
    {"RAM", "Flash"},{"Host", "ISA", "EISA-Bridge", "MicroChannel", "PCI-Bridge", "PCMCIA-Bridge", "NuBus-Bridge", "CardBus-Bridge", "Raceway", "Semitransparente PCI/PCI-Bridge","InfinityBand"}
};


/**
 * searches for a specific PCI device by vendor and device ID
 * @param device_list pci device list
 * @param vendor Vendor ID of the device
 * @param device devicde- ID of the device
 * @param num of the device in a device list
 * @return NULL if there is no more device of the specified type or the adress auf the PCI device struct
 */
struct pci_dev *pci_search_device(list_t *device_list, uint16_t vendor, uint16_t device, int num)
{
    while(device_list->lock){}
    device_list->lock = true;
        list_set_first(device_list);
        while(!list_is_last(device_list))
        {
            struct pci_dev *current_dev = list_get_current(device_list);
            if(num == 0 && current_dev->device_ID == device && current_dev->vendor_ID == vendor)
            {
                device_list->lock = false;
                return current_dev;
            }
            else if(current_dev->device_ID == device && current_dev->vendor_ID == vendor)
            {
                num--;
            }
            list_next(device_list);
        }
    device_list->lock = false;
    return NULL;
}

list_t *pci_irq_handles;

/**
 * installs a isr handler for a specific pci device
 * @param isr interrupt service routine
 * @param dev pci device
 */
void pci_install_isr(void (*isr)(struct pci_dev *dev), struct pci_dev *dev)
{
    while(pci_irq_handles->lock){}
    pci_irq_handles->lock = true;
        struct pci_isr *new_pci_isr = malloc(sizeof(struct pci_isr));
        new_pci_isr->isr = isr;
        new_pci_isr->dev = dev;
        list_push_front(pci_irq_handles,new_pci_isr);
    pci_irq_handles->lock = false;
}


/**
 * deinstalls a isr handler for a specific pci device
 * @param isr interrupt service routine
 * @param dev pci device
 */
int pci_deinstall_isr(void (*isr)(struct pci_dev *dev), struct pci_dev *dev)
{
    while(pci_irq_handles->lock){}
    pci_irq_handles->lock = true;
        int num = 0;
        list_set_first(pci_irq_handles);
        while(!list_is_last(pci_irq_handles))
        {
            struct pci_isr *current_isr = list_get_current(pci_irq_handles);
            if(current_isr->isr == isr && current_isr->dev == dev)
            {
                list_remove(pci_irq_handles);
                list_set_first(pci_irq_handles);
                num++;
            }
            else
                list_next(pci_irq_handles);
        }
    pci_irq_handles->lock = false;
    return num;
}



//Linked list of PCI devices
list_t *pci_dev_list;


/**
 * handels pci irqs
 */
void pci_irq_handler(void)
{
    struct list_node *backup = pci_dev_list->head;
    list_set_first(pci_dev_list);
    while(!list_is_last(pci_dev_list))
    {
        struct pci_dev * current_dev = list_get_current(pci_dev_list);
        uint16_t status = pci_readw(current_dev->bus, current_dev->dev, current_dev->func, PCI_STATUS);
        if(status & PCI_STATUS_INT)
        {
            list_set_first(pci_irq_handles);
            while(!list_is_last(pci_irq_handles))
            {
                struct pci_isr *current_isr = list_get_current(pci_irq_handles);
                if(current_isr->dev == current_dev)
                    current_isr->isr(current_dev);
                list_next(pci_irq_handles);
            }
            pci_writel(current_dev->bus, current_dev->dev, current_dev->func, PCI_STATUS, status ^ 1);
        }
        list_next(pci_dev_list);
    }
    //printf("PCI-IRQ!\n");
    pci_dev_list->head = backup;
}


#define PRINT_DEV_LIST


void INIT_PCI()
{
    #ifdef PRINT_DEV_LIST
        printf("PCI-devices:\n");
    #endif
    if(!pci_dev_list)
        pci_dev_list = list_create();
    if(!pci_irq_handles)
        pci_irq_handles = list_create();
    pci_dev_list->lock = true;

    int dev,bus,func;

    for(bus = 0; bus < 8; bus++)
    {

        for(dev = 0; dev < 32; dev++)
        {

            for(func = 0; func < 8; func ++)
            {

                if(pci_dev_exist(bus, dev, func))
                {
                    bool multifunc = (pci_readb(bus, dev, func,PCI_HEADERTYPE) & 0x80) >> 7;

                    if(func && ! multifunc)
                        continue;

                    struct pci_dev *current_dev = malloc(sizeof(struct pci_dev));

                    current_dev->bus = bus;
                    current_dev->dev = dev;
                    current_dev->func = func;

                    uint32_t classcode = pci_readl(bus, dev, 0, PCI_REVISION);

                    current_dev->reversion_ID = (uint8_t)classcode;
                    current_dev->programming_interface = (uint8_t) (classcode >> 8);
                    current_dev->sub_class = (uint8_t) (classcode >> 16);
                    current_dev->base_class = (uint8_t) (classcode >> 24);

                    current_dev->device_ID = pci_readw(bus, dev, func, PCI_DEVICE_ID);
                    current_dev->vendor_ID = pci_readw(bus, dev, func, PCI_VENDOR_ID);
                    current_dev->header_type = (pci_readb(bus, dev ,0, PCI_HEADERTYPE) | 0x80)^0x80;
                    current_dev->multifunc = multifunc;

                    uint32_t irq_info = pci_readl(bus, dev, func, PCI_INTERRUPT);
                    current_dev->irq_num = (uint8_t) irq_info;
                    current_dev->irq_pin = (uint8_t) (irq_info >> 8);
                    current_dev->locked = false;


                    #ifdef PRINT_DEV_LIST
                        printf("device ID: %04X  vendor ID: %04X  bus: %d  port: %d  function: %d interrupt:%d\n",current_dev->device_ID, current_dev->vendor_ID, current_dev->bus, current_dev->dev, current_dev->func, current_dev->irq_num);
                    #endif

                    if(current_dev->irq_num < 16 && current_dev->irq_pin && current_dev->irq_num)
                    {
                        if(!install_irq(current_dev->irq_num, pci_irq_handler))
                        {
                            set_color(RED | BLACK << 4);
                            printf("WARNING: ");
                            set_color(WHITE | BLACK << 4);
                            printf("Could not Reserver IRQ %d for PCI-DEVICE: %d:%d:%d\n", current_dev->irq_num, bus, dev, func);

                            set_color(RED | BLACK << 4);
                            printf("WARNING: ");
                            set_color(WHITE | BLACK << 4);
                            printf("FORCING IRQ %d for the PCI-BUS\n", current_dev->irq_num);

                            deinstall_irq(current_dev->irq_num);
                            install_irq(current_dev->irq_num, pci_irq_handler);

                        }

                    }

                    // Standart Device
                    if(! (current_dev->header_type & 0xFF) )
                    {
                        int base;

                        for(base = 0; base < 6; base++)
                        {
                            uint32_t current_base = pci_readl(bus, dev, func, PCI_BASE + (base * 4));

                            // get type
                            current_dev->base_adress[base].type = current_base & 1;

                            // save current adress
                            current_dev->base_adress[base].adress = (current_base | 1) ^ 1;

                            // get reserved bits
                            pci_writel(bus, dev, func, PCI_BASE + (base * 4), 0xFFFFFFFF);
                            uint32_t temp_base = pci_readl(bus, dev, func, PCI_BASE + (base * 4));
                            if(temp_base == 0)
                                current_dev->base_adress[base].type = UNUSED;
                            temp_base = (~temp_base) | 1;
                            current_dev->base_adress[base].resb = 0;
                            int i;
                            for(i = 0; i < 32; i++)
                            {
                                if((temp_base & (1 << i)) != 0)
                                    current_dev->base_adress[base].resb++;
                                else
                                    break;
                            }

                            //reset old state
                             pci_writel(bus, dev, func, PCI_BASE + (base * 4), current_base);
                        }


                    }
                    // Bridge Device
                    else
                    {
                        //TODO: Write Cases for Bridges
                    }

                    list_push_front(pci_dev_list, current_dev);
                }
            }
        }
    }
    pci_dev_list->lock = false;

    #ifdef PRINT_DEV_LIST
        printf("\n");
    #endif
}
