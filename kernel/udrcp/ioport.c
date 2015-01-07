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

#include <printf.h>
#include <udrcp/hypervisor.h>
#include <udrcp/ioport.h>
#include <io.h>
#include <sched/thread.h>

/**
 * @brief handles PORT_ALLOC call
 * @param drv pointer to the driver struct
 * @param req pointer to the request package
 */
void handle_port_alloc(struct driver *drv, pck_t *req)
{
    //validate package
    if(req->size != sizeof(portpck_t)+12)
    {
        printf("invalid package! %d  %d\n",req->size,(int)sizeof(portpck_t));
        respond(drv->pman, req->id, INVALID_REQUEST, 0, 0);
        free(req);
        return;
    }

    portid_t port = (portid_t) *((unsigned int*)req->data);
    if(alloc_port(&drv->process->main_thread->context, port))
    {
        list_push_front(drv->ports,req->data);
        port_type p = hw_port;
        respond(drv->pman, req->id, SUCCESS, sizeof(port_type), &p);
    }
    else
    {
        unsigned int resp = 0;
        respond(drv->pman, req->id, ERROR, sizeof(port_type), &resp );
    }


}



/**
 * @brief handles PORT_FREE call
 * @param drv pointer to the driver struct
 * @param req pointer to the request package
 */
void handle_port_free(struct driver *drv, pck_t *req)
{
    iterator_t i = iterator_create(drv->ports);
    portpck_t *free_port = (portpck_t*) req->data;
    while(!list_is_last(&i))
    {
        if(((portpck_t*)list_get_current(&i))->port == free_port->port)
        {
            free(list_remove(&i));
            respond(drv->pman, req->id, SUCCESS, 0, 0);
            return;
        }
        list_next(&i);
    }

    respond(drv->pman, req->id, ERROR, 0, 0);
}


