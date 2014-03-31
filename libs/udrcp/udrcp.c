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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pckmgr *new_pckmgr(int in, int out, int err)
{
    pckmgr *mgr = malloc(sizeof(pckmgr));
    mgr->counter = 0;
    mgr->used_ids = list_create();
    mgr->in = in;
    mgr->out = out;
    mgr->err = err;
    mgr->recieved_pcks = list_create();
    return mgr;
}

pckid_t gen_pckid(pckmgr *mgr)
{
    list_set_first(mgr->used_ids);
    while(!list_is_empty(mgr->used_ids) && !list_is_last(mgr->used_ids))
    {
        if((pckid_t)list_get_current(mgr->used_ids) == mgr->counter )
        {
            if(mgr->counter == MAX_ID)
                mgr->counter = 0;
            else
                mgr->counter++;

            list_set_first(mgr->used_ids);
        }
        else
            list_next(mgr->used_ids);
    }
    list_push_front(mgr->used_ids,(void*)mgr->counter);
    return mgr->counter++;
}


int free_pckid(pckmgr *mgr, pckid_t id)
{
    list_set_first(mgr->used_ids);
    while(!list_is_empty(mgr->used_ids) && !list_is_last(mgr->used_ids))
    {
        if((pckid_t)list_get_current(mgr->used_ids) == id )
        {
            list_remove(mgr->used_ids);
            return true;
        }
        else
            list_next(mgr->used_ids);
    }

    return false;
}

void free_pck(pck_t *package)
{
    if(package->data > 0)
        free(package->data);
    free(package);
}

pckid_t send_package(pckmgr *mgr, pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    pckid_t id = gen_pckid(mgr);
    header->id = id;
    header->size = size+12;
    header->type = type;
    write(mgr->out,header,sizeof(pckhead_t));
    write(mgr->out,data,size);
    free(header);
    return id;
}

void respond(pckmgr *mgr,pckid_t id,pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    header->id = id;
    header->size = size+12;
    header->type = type;
    write(mgr->out,header,sizeof(pckhead_t));
    write(mgr->out,data,size);
    free(header);
}

int subsystem_connect(pckmgr *mgr, char *protocol_version)
{
    send_package(mgr, RESET_CON, 0, NULL);
    pckid_t ping_id = send_package(mgr, PING, strlen(protocol_version), protocol_version);
    pckhead_t *pck_header = malloc(sizeof(pckhead_t));
    read(mgr->in,pck_header,sizeof(pckhead_t));
    if(pck_header->id == ping_id && pck_header->type == PONG)
    {
        mgr->version = malloc(pck_header->size);
        read(mgr->in,mgr->version,pck_header->size-sizeof(pckhead_t));
    }
    else
    {
        write(mgr->err,"invalind ping response from host",32);
        free(pck_header);
        return false;
    }
    //todo: check udrcp version
    free(pck_header);
    return true;
}


pck_t *poll_next(pckmgr *mgr)
{
    pck_t *pck = malloc(sizeof(pck_t));
    read(mgr->in,pck,sizeof(pckhead_t));
    if(pck->size > 12)
    {
        pck->size -= 12;
        pck->data = malloc(pck->size);
        read(mgr->in,pck->data,pck->size);
    }
    else
        pck->data = NULL;
    return pck;
}


void poll_queue(pckmgr *mgr)
{
    list_push_front(mgr->recieved_pcks,poll_next(mgr));
}

pck_t *fetch_queue(pckmgr *mgr,pckid_t id)
{
    list_set_first(mgr->recieved_pcks);
    while(!list_is_last(mgr->recieved_pcks) && !list_is_empty(mgr->recieved_pcks))
    {
        pck_t *current = list_get_current(mgr->recieved_pcks);
        if(current->id == id)
        {
            list_remove(mgr->recieved_pcks);
            return current;
        }
        list_next(mgr->recieved_pcks);
    }
    return NULL;
}


pck_t *pck_poll(pckmgr *mgr, pckid_t id)
{
    pck_t *pck = fetch_queue(mgr,id);
    if(pck)
        return pck;

    pck = poll_next(mgr);
    while(pck->id != id)
    {
        list_push_front(mgr->recieved_pcks,pck);
        pck = poll_next(mgr);
    }
    return pck;
}


void reset_conn(pckmgr *mgr)
{
    mgr->counter = 0;
    while(!list_is_empty(mgr->used_ids))
        free_pck(list_pop_front(mgr->used_ids));
    while(!list_is_empty(mgr->recieved_pcks))
        free_pck(list_pop_front(mgr->recieved_pcks));
}



int req_intsig(pckmgr *mgr, unsigned int num)
{
    pckid_t id = send_package(mgr, INT_REQ, sizeof(unsigned int), &num);
    int errorcode = true;
    while(errorcode)
    {
        pck_t *resp = pck_poll(mgr,id);
        switch(resp->type)
        {
            case ERROR:
                if(resp->size >= 4)
                    errorcode = *((unsigned int*)resp->data);
                else
                    errorcode = -1;
            break;

            case CONFIRM:
            break;

            case SUCCESS:
                free_pck(resp);
                return true;
            break;

            default:
                write(mgr->err,"unknown response\n",18);
                errorcode = -2;
            break;

        }
        //free_pck(resp);
    }
    return errorcode;
}

int free_intsig(pckmgr *mgr, unsigned int num)
{
    pckid_t id = send_package(mgr, INT_FREE, sizeof(unsigned int), &num);
    int errorcode = true;
    while(errorcode)
    {
        pck_t *resp = pck_poll(mgr,id);
        switch(resp->type)
        {
            case ERROR:
                if(resp->size >= 4)
                    errorcode = *((unsigned int*)resp->data);
                else
                    errorcode = -1;
            break;

            case CONFIRM:
            break;

            case SUCCESS:
                free_pck(resp);
                return true;
            break;

            default:
                write(mgr->err,"unknown response\n",18);
                errorcode = -2;
            break;

        }
        free_pck(resp);
    }
    return errorcode;
}



