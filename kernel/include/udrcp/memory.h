#ifndef _memory_h_
#define _memory_h_
/*
     Copyright 2014 Infinitycoding all rights reserved
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
 **/

#include <udrcp/udrcp.h>
#include <udrcp/hypervisor.h>


typedef enum
{
    PMA_ALLOC_RANDOM = 1,
    PMA_ALLOC_SPECIFIC = 2,
    PMA_ALLOC_LOWER = 3,
}pma_t;

struct pma_request
{
    pma_t type;
    size_t size;
    paddr_t phys_base;
};

struct pma_response
{
    vaddr_t mem_base;
    paddr_t phys_base;
};

void handle_pma_alloc(struct driver *drv, pck_t *req);
void handle_pma_free(struct driver *drv, pck_t *req);


 #endif