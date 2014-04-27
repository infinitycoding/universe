#ifndef _cdi_h_
#define _cdi_h_
/*
     Copyright 2012-2014 Infinitycoding all rights reserved
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



/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  based on the CDI reference implementation by Kevin Wolf
 */

#include <cdi/lists.h>
#include <stdlib.h>

typedef enum {
    CDI_UNKNOWN         = 0,
    CDI_NETWORK         = 1,
    CDI_STORAGE         = 2,
    CDI_SCSI            = 3,
    CDI_VIDEO           = 4,
    CDI_AUDIO           = 5,
    CDI_AUDIO_MIXER     = 6,
    CDI_USB_HCD         = 7,
    CDI_USB             = 8,
    CDI_FILESYSTEM      = 9,
    CDI_PCI             = 10,
}cdi_device_type_t;

struct cdi_driver;

struct cdi_driver
{
    cdi_device_type_t type;
    const char        *name;
    cdi_list_t        devices;
    void (*init_device)(struct cdi_device *device);
    void (*remove_device)(struct cdi_device *device);
    void (*destroy)(struct cdi_driver *driver);
};

struct cdi_bus_data {
    cdi_device_type_t   bus_type;
};

struct cdi_device{
    cdi_device_type_t type;
    const char        *name;
    struct cdi_driver *driver;
};


void cdi_init(void);
void cdi_driver_init(struct cdi_driver *driver);
void cdi_driver_destroy(struct cdi_driver *driver);
#endif
