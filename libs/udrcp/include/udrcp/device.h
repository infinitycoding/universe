#ifndef _udrcp_dev_h_
#define _udrcp_dev_h_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the UDRCP-library.

     The UDRCP-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The UDRCP-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with the UDRCP-library.  If not, see <http://www.gnu.org/licenses/>.
 */



/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */

#include <udrcp/ioport.h>
#include <udrcp/memory.h>

#define UDRCP_BLOCK_DEV 0
#define UDRCP_NE


// PCI-Structs
typedef enum
{
    MEMORY = 0,
    PORT = 1,
} pci_res_t;

typedef enum
{
    CDI_PCI_MEMORY,
    CDI_PCI_IOPORTS
} cdi_res_t;

struct cdi_pci_resource
{
    cdi_res_t    type;
    uintptr_t    start;
    size_t       length;
    unsigned int index;
    void         *address;
};

struct pci_device
{
    uint16_t bus;
    uint16_t dev;
    uint16_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_id;
    uint8_t  subclass_id;
    uint8_t  interface_id;
    uint8_t  rev_id;
    uint8_t  irq;
    cdi_list_t resources;
};





#endif