#ifndef _ioport_h_
#define _ioport_h_
/*
	Copyright 2012 universe coding group (UCG) all rights reserved
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

**/

/**
  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
  */

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
}port_t;

port_t *port_alloc(pckmgr *mgr,unsigned int port);
void port_free(port_t *p);

unsigned char port_inb(port_t *p);
unsigned short port_inw(port_t *p);
unsigned long port_inl(port_t *p);

int port_outb(port_t *p, unsigned char v);
int port_outw(port_t *p, unsigned short v);
int port_outl(port_t *p, unsigned long v);

#endif
