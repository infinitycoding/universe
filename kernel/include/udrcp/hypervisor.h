#ifndef _HYPERVISOR_H_
#define _HYPERVISOR_H_
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

#include <udrcp/udrcp.h>
#include <sched/process.h>

struct int_relation
{
    unsigned int intnr;
    pckmgr *drv;
};

struct driver
{
    struct process_state *process;
    list_t *memory;
    list_t *ports;
    pckmgr *pman;
};


void INIT_HYPERVISOR(int argc, void **argv);



#endif
