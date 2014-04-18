#ifndef _cdi_osdep_h_
#define _cdi_osdep_h_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the CDI-wrapper-library.
 
     The CDI-wrapper-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The CDI-wrapper-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the CDI-wrapper-library.  If not, see <http://www.gnu.org/licenses/>.
 */



/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  based on the CDI reference implementation by Kevin Wolf
 */

#include <stdio.h>

#define CDI_STANDALONE
#define UNIVERSE

#define CDI_DRIVER(name, drv,...) // to-do: write the drivers to a specific section

typedef struct
{
} cdi_pci_device_osdep;

typedef struct
{
    FILE* file;
} cdi_dma_osdep;

typedef struct {
    int mapped;
} cdi_mem_osdep;

typedef struct {
    FILE* device;
} cdi_fs_osdep;

#endif