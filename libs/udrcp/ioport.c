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
#include <universe.h>




static inline void hw_outb(uint16_t port, uint8_t value) {
	asm volatile("outb %1, %0" : : "Nd" (port), "a" (value));
}

static inline void hw_outw(uint16_t port, uint16_t value) {
	asm volatile("outw %1, %0" : : "Nd" (port), "a" (value));
}

static inline void hw_outl(uint16_t port, uint32_t value) {
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
            break;

            case CONFIRM:
            break;

            case SUCCESS:
                port = (port_t *)malloc(sizeof(port_t));
                port->type = *((port_type*)response->data);
                port->mgr = mgr;
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
            break;

            default:
                write(stderr,"error: unsupported response\n",28);
                exit(1);
            break;
       };
   }
   return NULL; // just to keep the compiler happy
}

bool port_free(port_t *p)
{
    pckid_t id = send_package(p->mgr,PORT_FREE,sizeof(unsigned int),&p->port);
    if(p->type == file_port)
        close(p->port);
    free(p);
    while(1)
    {
        pck_t *response = pck_poll(p->mgr,id);
        switch(response->type)
        {
            case ERROR:
                return false;
            break;

            case CONFIRM:
            break;

            case SUCCESS:
                return true;
            break;

            default:
                write(stderr,"error: unsupported response\n",28);
                exit(1);
            break;
        };
    }
}



unsigned char inb(port_t *p)
{
    unsigned char result;
    pck_t *response;
    pckid_t id;
    switch(p->type)
    {
        case hw_port:
            asm volatile("inb %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            id = send_package(p->mgr,VPORT_R,sizeof(unsigned int),&p->port);
            while(1)
            {
                response = pck_poll(p->mgr,id);
                switch(response->type)
                {
                    case ERROR:
                        write(stderr,"could not read from port\n",24);
                        exit(1);
                    break;

                    case CONFIRM:
                    break;

                    case SUCCESS:
                        return *response->data;
                    break;
                };
            }
            break;

        case file_port:
            read(p->port,&result,sizeof(unsigned char));
            return result;
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;

    };
    return result; // just to keep the compiler happy
}

unsigned short inw(port_t *p)
{
    unsigned short result;
    pck_t *response;
    pckid_t id;
    switch(p->type)
    {
        case hw_port:
            asm volatile("inw %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            id = send_package(p->mgr,VPORT_R,sizeof(unsigned int),&p->port);
            while(1)
            {
                response = pck_poll(p->mgr,id);
                switch(response->type)
                {
                    case ERROR:
                        write(stderr,"could not read from port\n",25);
                        exit(1);
                    break;

                    case CONFIRM:
                    break;

                    case SUCCESS:
                        return *((unsigned short*)response->data);
                    break;
                };
            }
            break;

        case file_port:
            read(p->port,&result,sizeof(unsigned short));
            return result;
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;

    };
    return result; // just to keep the compiler happy
}

unsigned long inl(port_t *p)
{
    unsigned long result;
    pck_t *response;
    pckid_t id;
    switch(p->type)
    {
        case hw_port:
            asm volatile("inl %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            id = send_package(p->mgr,VPORT_R,sizeof(unsigned int),&p->port);
            while(1)
            {
                response = pck_poll(p->mgr,id);
                switch(response->type)
                {
                    case ERROR:
                        write(stderr,"could not read from port\n",25);
                        exit(1);
                    break;

                    case CONFIRM:
                    break;

                    case SUCCESS:
                        return *((unsigned long*)response->data);
                    break;
                };
            }
            break;

        case file_port:
            read(p->port,&result,sizeof(unsigned long));
            return result;
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;

    };
    return result; // just to keep the compiler happy
}




