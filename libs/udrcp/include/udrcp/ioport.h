#ifndef _ioport_h_
#define _ioport_h_
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

typedef enum
{
    hw_port,
    host_port,
    file_port,
}port_type;


typedef struct
{
    port_type type;
    unsigned int port;
    pckmgr *mgr;
}port_t;


typedef struct
{
    port_type type;
    unsigned int port;
}port_resp;


typedef struct
{
    unsigned int port;
}port_req;


port_t *port_alloc(pckmgr *mgr,unsigned int port);
int port_free(port_t *p);

unsigned char inb(port_t *p);
unsigned short inw(port_t *p);
unsigned long inl(port_t *p);

void outb(port_t *p, unsigned char v);
void outw(port_t *p, unsigned short v);
void outl(port_t *p, unsigned long v);

#endif
