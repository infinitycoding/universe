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


#include <ioport.h>
#include <stdlib.h>
#include <unistd.h>

static inline uint8_t inb(uint16_t port) {
	uint8_t result;
	asm volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

static inline uint16_t inw(uint16_t port) {
	uint16_t result;
	asm volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

static inline uint32_t inl(uint16_t port) {
	uint32_t result;
	asm volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

static inline void outb(uint16_t port, uint8_t value) {
	asm volatile("outb %1, %0" : : "Nd" (port), "a" (value));
}

static inline void outw(uint16_t port, uint16_t value) {
	asm volatile("outw %1, %0" : : "Nd" (port), "a" (value));
}

static inline void outl(uint16_t port, uint32_t value) {
	asm volatile("outl %1, %0" : : "Nd" (port), "a" (value));
}


port_t *port_alloc(pckmgr *mgr,unsigned int port_nr)
{
   pckid_t id = send_package(mgr,PORT_ALLOC,sizeof(unsigned int),&port_nr);
   port_t *port;
   while(1)
   {
       pck_t *response = pck_poll(mgr,id);
       switch(response->type)
       {
            case ERROR:
                return (port_t*)(*((unsigned int*)response->data));
            continue;

            case CONFIRM:
            continue;

            case SUCCESS:
                port = (port_t *)malloc(sizeof(port_t));
                port->type = *((port_type*)response->data);
                if(port->type == file_port)
                {
                    port->port = open((const char*)(response->data+sizeof(port_type)),3,0);
                    if(port->port <= 0)
                    {
                        free(port);
                        return NULL;
                    }
                    return port;
                }
                port->port = port_nr;
                return port;
            continue;
       };
   }
   return NULL; // just to keep the compiler happy
}
