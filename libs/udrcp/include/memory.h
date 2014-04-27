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

#include <udrcp.h>

{
    pma_mem,
    shm_mem,
}mem_type;



typedef struct
{
     mem_type type;
     void *membase;
     void *phys_base;
     unsigned int range;
}mem_t;


mem_t *alloc_shm(pckmgr *conn, size_t size);
mem_t *alloc_pma(pckmgr *conn, size_t size);
mem_t *alloc_pma_area(pckmgr *conn, void *phys_base, size_t size);

int share_block(pckmgr *conn, mem_t *mem, size_t range);
int hand_block(pckmgr *conn, mem_t *mem, size_t range);

int free_shm(mem_t *mem);
int free_pma(mem_t *mem);

 #endif