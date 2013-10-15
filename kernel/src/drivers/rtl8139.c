#include <drivers/pci.h>
#include <drivers/rtl8139.h>
#include <io.h>


list_t *rtl_dev = NULL;
int rtl_devc = 0;


void INIT_RTL8139(void)
{
    printf("RTL8139:\n");
    rtl_dev = list_create();
    struct pci_dev *current_dev = pci_search_device(pci_dev_list, VENDOR_ID, DEVICE_ID, rtl_devc);
    while(current_dev != NULL)
    {
        rtl_devc++;
        list_push_front(rtl_dev, current_dev);


        uint32_t port_base = 0;
        int i;
        for(i = 0; i < 6; i++)
        {
            if(current_dev->base_adress[i].type == PORT)
            {
                port_base = current_dev->base_adress[i].adress;
                break;
            }
        }
        if(port_base == 0)
            printf("invalid device; no port base found!\n");


        // require ports from kernel manager
        pci_install_isr(rtl8139_isr, current_dev);

        current_dev = pci_search_device(pci_dev_list, VENDOR_ID, DEVICE_ID, rtl_devc);
    }



}


void rtl8139_isr(struct pci_dev *dev)
{
    printf("RTL IRQ!\n");
}
