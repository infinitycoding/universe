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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <udrcp/ioport.h>



// Todo: error handling for hosted ports


port_t *port_alloc(pckmgr *mgr,unsigned int port_nr)
{
    port_req req;
    req.port = port_nr;
    pckid_t id = send_package(mgr,PORT_ALLOC,sizeof(unsigned int),&req);
    port_resp *resp;
    port_t *port;
    int retv;
    while(1)
    {
        pck_t *response = pck_poll(mgr,id);
        switch(response->type)
        {
             case ERROR:
                 retv = *((unsigned int*)response->data);
                 free(response->data);
                 free(response);
                 return (port_t*)retv;
             break; 

             case INVALID_REQUEST:
                 free(response->data);
                 free(response);
                 return NULL;
             break;

             case CONFIRM:
                 free(response);
             break;

             case SUCCESS:
                 port = (port_t *)malloc(sizeof(port_t));
                 resp = (port_resp*)response->data;
                 
                 port->type = resp->type;
                 port->mgr = mgr;

                 if(port->type == file_port)
                 {
                     port->port = open((char*)resp->port, 3, 0);
                 }
                 else if(port->type == hw_port || port->type == host_port)
                 {
                     port->port = port_nr;
                 }
                 else
                 {
                    udrcp_error(mgr, "error: unsupported port type %d\n",port->type);
                    free(port);
                    port = NULL;
                 }

                 free(response->data);
                 free(response);
                 return port;

             break;

             default:
                udrcp_error(mgr, "error: unsupported response type %d\n",response->type);
                 exit(1);
             break;
        };
    }
    return NULL; // just to keep the compiler happy
}


int port_free(port_t *p)
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
                free(response);
                return false;
            break;

            case CONFIRM:
                free(response);
            break;

            case SUCCESS:
                free(response);
                return true;
            break;

            default:
                write(stderr,"error: unsupported response\n",28);
                exit(1);
            break;
        };
    }
}


int host_in(port_t *p,void *data,int size)
{
    port_req req;
    req.port = p->port;
    pckid_t id = send_package(p->mgr,VPORT_R,sizeof(port_req),&req);
    pck_t *response;
    int retv;
    while(1)
    {
        response = pck_poll(p->mgr,id);
        switch(response->type)
        {
            case ERROR:
                retv = *((unsigned int*)response->data);
                free(response->data);
                free(response);
                return retv;
            break;

            case CONFIRM:
                free(response);
            break;

            case SUCCESS:
                memcpy(data,response->data,size);
                free(response->data);
                free(response);
                return true;
            break;
        };
    }
}


unsigned char inb(port_t *p)
{
    unsigned char result;
    switch(p->type)
    {
        case hw_port:
            asm volatile("inb %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            host_in(p,&result,sizeof(unsigned char));
            return result;
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
    switch(p->type)
    {
        case hw_port:
            asm volatile("inw %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            host_in(p,&result,sizeof(unsigned short));
            return result;
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
    switch(p->type)
    {
        case hw_port:
            asm volatile("inl %1, %0" : "=a" (result) : "Nd" ((unsigned short)p->port));
            return result;
        break;

        case host_port:
            host_in(p,&result,sizeof(unsigned long));
            return result;
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


int host_out(port_t *p, void *value, int size)
{
    pck_t *response;
    pckid_t id;
    unsigned int *buffer = malloc(size+sizeof(unsigned int));
    buffer[0] = p->port;
    memcpy(buffer + sizeof(unsigned int),value,size);
    id = send_package(p->mgr,VPORT_W,size+sizeof(unsigned int),buffer);
    free(buffer);
    int retv;
    while(1)
    {
        response = pck_poll(p->mgr,id);
        switch(response->type)
        {
            case ERROR:
                retv = *((unsigned int*)response->data);
                free(response->data);
                free(response);
                return retv;
            break;

            case CONFIRM:
                free(response);
            break;

            case SUCCESS:
                free(response->data);
                free(response);
                return true;
            break;
        };
    }
    return retv;
}


void outb(port_t *p, unsigned char v)
{
    switch(p->type)
    {
        case hw_port:
            asm volatile("outb %1, %0" : : "Nd" ((uint16_t)p->port), "a" (v));
        break;

        case host_port:
            host_out(p, &v, sizeof(unsigned char));
        break;

        case file_port:
            write(p->port,&v,sizeof(unsigned char));
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;
    };
}



void outw(port_t *p, unsigned short v)
{
    switch(p->type)
    {
        case hw_port:
            asm volatile("outw %1, %0" : : "Nd" ((uint16_t)p->port), "a" (v));
        break;

        case host_port:
            host_out(p, &v, sizeof(unsigned short));
        break;

        case file_port:
            write(p->port,&v,sizeof(unsigned short));
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;
    };
}



void outl(port_t *p, unsigned long v)
{
    switch(p->type)
    {
        case hw_port:
            asm volatile("outl %1, %0" : : "Nd" ((uint16_t)p->port), "a" (v));
        break;

        case host_port:
            host_out(p, &v, sizeof(unsigned long));
        break;

        case file_port:
            write(p->port,&v,sizeof(unsigned long));
        break;

        default:
            write(stderr,"error: a unsupported port-type is used\n",40);
            exit(1);
        break;
    };
}
