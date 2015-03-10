#include <pci.h>
#include <udrcp/ioport.h>


 char *pci_classes[] = {
    "storage", "network", "display", "multimedia", "memory",
    "bridge", "bus/port", "perephiery", "input", "docking station",
    "processor", "serial bus", "wireless", "intelegent", "Satelite",
    "crypto", "signal processor"
 };


/**
 * Collection of pci device names orderd by class and subclass pci_dev_names[class][subclass]
 */
char *pci_dev_names[][16]={
    {"non vga device", "vga device"}, {"SCSI", "IDE", "Floppy", "IPI", "RAID", "ATA-controler", "SATA-controler","SAS"},
    {"Ethernet", "Token Ring", "FDDI", "ATM", "ISDN", "FIP", "PICMG 2.14"}, {"VGA", "XGA", "3D"},
    {"Video", "Audio", "Phone", "HD-Audio"},{"RAM", "Flash"},{"Host", "ISA", "EISA-Bridge", "MicroChannel", "PCI-Bridge", "PCMCIA-Bridge", "NuBus-Bridge", "CardBus-Bridge", "Raceway", "Semitransparente PCI/PCI-Bridge","InfinityBand"}
};

extern port_t *address_port;
extern port_t *data_port;

uint32_t pci_readl(uint8_t bus,uint8_t dev,uint8_t func,uint8_t offset)
{
    outl(address_port,0x80000000 | (bus << 16) | (dev << 11) |( func << 8) | (offset & 0xFC));
    return inl(data_port) >> (8 * (offset % 4));
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
    return pci_readl(bus, dev, func, offset) & 0xff;
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
    return pci_readl(bus, dev, func, offset) & 0xffff;
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
        outl(address_port, 0x80000000 | (bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC));
            outl(data_port, value);
}


/**
 * checks if PCI device exists
 * @param Bus
 * @param Device/Slot
 * @return true (device exists) or false (device does not exist)
 */
bool pci_dev_exist(uint8_t bus, uint8_t dev, uint8_t func)
{
    uint16_t vendor_ID = pci_readw(bus, dev, func, 0);
    if(vendor_ID == 0 || vendor_ID == 0xFFFF)
        return false;
    return true;
}

void INIT_PCI()
{
    printf("PCI-devices:\n");
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
                    current_dev->header_type = (pci_readb(bus, dev ,0, PCI_HEADERTYPE) | 0x80)^0x80;
                    current_dev->multifunc = multifunc;

                    uint32_t irq_info = pci_readl(bus, dev, func, PCI_INTERRUPT);
                    current_dev->irq_num = (uint8_t) irq_info;
                    current_dev->irq_pin = (uint8_t) (irq_info >> 8);
                    current_dev->locked = false;
                    printf("device ID: %04X  vendor ID: %04X  bus: %d  port: %d  function: %d interrupt:%d\n",current_dev->device_ID, current_dev->vendor_ID, current_dev->bus, current_dev->dev, current_dev->func, current_dev->irq_num);
                    if(! (current_dev->header_type & 0xFF) )
                    {
                        int base;
                        for(base = 0; base < 6; base++)
                        {
                            uint32_t current_base = pci_readl(bus, dev, func, PCI_BASE + (base * 4));
                            //get type
                            current_dev->base_adress[base].type = current_base & 1;
                            current_dev->base_adress[base].adress = (current_base | 1) ^ 1;
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
                }
            }
        }
    }
}