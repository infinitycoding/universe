#ifndef _pci_h_
#define _pci_h_

#include <stdint.h>
#include <stdbool.h>


#define PCI_CONFIG_DATA 0x0CFC
#define PCI_CONFIG_ADDRESS 0x0CF8

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION 0x08
#define PCI_CLASS 0x0B
#define PCI_SUBCLASS 0x0A
#define PCI_INTERFACE 0x09
#define PCI_HEADERTYPE 0x0E
#define PCI_BASE 0x10
#define PCI_INTERRUPT 0x3C
#define PCI_STATUS_INT (1 << 3)
#define PCI_STATUS_CAP_LST (1 << 4)
#define PCI_STATUS_66MHZ (1 << 5)
#define PCI_STATUS_F2B (1 << 7)
#define PCI_STATUS_MDPR (1 << 8)
#define PCI_COMMAND_IO 1
#define PCI_COMMAND_MEM (1 << 1)
#define PCI_COMMAND_MASTER (1 << 2)
#define PCI_COMMAND_SPEC_CYC (1 << 3)
#define PCI_COMMAND_MEM_INVAL (1 << 4)
#define PCI_COMMAND_VGA_SNOOP (1 << 5)
#define PCI_COMMAND_PARITY_ERR (1 << 6)
#define PCI_COMMAND_SERR (1 << 8)
#define PCI_COMMAND_B2B (1 << 9)
#define PCI_COMMAND_INT (1 << 10)
#define PCI_STANDART_HEADER 0
#define PCI_PCI_BRIDGE_HEADER 1
#define PCI_CARDBUS_BRIDGE_HEADER 2




typedef enum
{
  INTA = 1,
  INTB = 2,
  INTC = 3,
  INTD = 4,
}irq_pin;

typedef enum
{
  MEMORY = 0,
  PORT = 1,
  UNUSED = 2,
}pci_base_t;

struct pci_dev_base
{
  uint32_t adress;
  pci_base_t type;
  uint8_t resb;
};

struct pci_dev
{
  uint8_t bus;
  uint8_t dev;
  uint8_t func;
  bool locked;
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

struct pci_isr
{
  void (*isr)(struct pci_dev *dev);
  struct pci_dev *dev;
};

void INIT_PCI();
bool pci_dev_exist(uint8_t bus, uint8_t dev, uint8_t func);
#endif