#include <drivers/pci.h>
#include <drivers/rtl8139.h>
#include <printf.h>
#include <heap.h>
#include <math.h>


list_t *rtl_dev = NULL;
int rtl_devc = 0;


void INIT_RTL8139(void)
{
    printf("RTL8139:\n");
    rtl_dev = list_create();
    struct pci_dev *current_pci_dev = pci_search_device(pci_dev_list, VENDOR_ID, DEVICE_ID, rtl_devc);
    while(current_pci_dev != NULL)
    {
        rtl_devc++;
        struct rtl_dev *current_dev = malloc(sizeof(struct rtl_dev ));
        current_dev->dev = current_pci_dev;
        list_push_front(rtl_dev,(void *) current_dev);


        int i;
        for(i = 0; i < 6; i++)
        {
            if(current_pci_dev->base_adress[i].type == PORT)
            {
                current_dev->portbase = current_pci_dev->base_adress[i].adress;
                // TODO: require ports from kernel manager
                continue;
            }
            else if(current_pci_dev->base_adress[i].type == MEMORY)
            {
                // TODO: require memory
                current_dev->phys_membase = 0xC00000; // A Random adress because there is no reall alloc function
                current_dev->virt_membase = 0xC0C00000;
                pci_writel(current_pci_dev->bus, current_pci_dev->dev, current_pci_dev->func, PCI_BASE + (i*4), current_dev->phys_membase);
            }
        }
        if(current_dev->portbase == 0)
            printf("invalid device; no port base found!\n");


        rtl8139_reset(current_dev);

        pci_install_isr(rtl8139_isr, current_pci_dev);

        current_pci_dev = pci_search_device(pci_dev_list, VENDOR_ID, DEVICE_ID, rtl_devc);
    }



}

void rtl8139_reset(struct rtl_dev * dev)
{
    rtl_outb(dev,REG_COMMAND, RESET_CARD); // perform reset
    while(rtl_inb(dev, REG_COMMAND) & RESET_CARD){}
    rtl_outb(dev, REG_COMMAND, ENABLE_RECIEVER | ENABLE_TRANSMITTER); // enable reciever and transmitter
    rtl_outl(dev, REG_TRANSMIT_CONFIGURATION, 0x03000700);
    rtl_outl(dev, REG_RECEIVE_CONFIGURATION, 0x0000070a);
    rtl_outl(dev, REG_RECEIVE_BUFFER, dev->phys_membase); // set recieve buffer
    rtl_outw(dev, REG_INTERRUPT_MASK, ALL_INTERRUPTS); //Allow all interrupts
}


void rtl8139_isr(struct pci_dev *dev)
{
    printf("RTL IRQ!\n");
}

