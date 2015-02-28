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



pckmgr *new_pckmgr(vfs_inode_t *in, vfs_inode_t *out, vfs_inode_t *err)
{
    pckmgr *mgr = malloc(sizeof(pckmgr));
    mgr->counter = 0;
    mgr->used_ids = list_create();
    mgr->pset.stdin = in;
    mgr->pset.stdout = out;
    mgr->pset.stderr = err;
    mgr->recieved_pcks = list_create();
    mgr->stdout_seeker = out->length;
    mgr->stderr_seeker = err->length;
    return mgr;
}

void reset_conn(pckmgr *mgr)
{
    mgr->counter = 0;
    while(!list_is_empty(mgr->used_ids))
        list_pop_front(mgr->used_ids);
    while(!list_is_empty(mgr->recieved_pcks))
        list_pop_front(mgr->recieved_pcks);
}


pckid_t gen_pckid(pckmgr *mgr)
{
    iterator_t used_id_it = iterator_create(mgr->used_ids);
    while(!list_is_empty(mgr->used_ids) && !list_is_last(&used_id_it))
    {
        if((pckid_t)list_get_current(&used_id_it) == mgr->counter )
        {
            if(mgr->counter == MAX_ID)
                mgr->counter = 0;
            else
                mgr->counter++;

            list_set_first(&used_id_it);
        }
        else
            list_next(&used_id_it);
    }
    list_push_front(mgr->used_ids,(void*)mgr->counter);
    return mgr->counter++;
}


bool free_pckid(pckmgr *mgr, pckid_t id)
{
    iterator_t used_id_it = iterator_create(mgr->used_ids);
    while(!list_is_empty(mgr->used_ids) && !list_is_last(&used_id_it))
    {
        if((pckid_t)list_get_current(&used_id_it) == id )
        {
            list_remove(&used_id_it);
            return true;
        }
        else
            list_next(&used_id_it);
    }

    return false;
}

pckid_t send_package(pckmgr *mgr, pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    pckid_t id = gen_pckid(mgr);
    header->id = id;
    header->size = size+sizeof(pckhead_t);
    header->type = type;
    vfs_write(mgr->pset.stdin,mgr->pset.stdin->length,header,sizeof(pckhead_t));
    if(size)
        vfs_write(mgr->pset.stdin,mgr->pset.stdin->length,data,size);
    free(header);
    return id;
}


void respond(pckmgr *mgr,pckid_t id,pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    header->id = id;
    header->size = size+12;
    header->type = type;
    vfs_write(mgr->pset.stdin,mgr->pset.stdin->length,header,sizeof(pckhead_t));
    if(size)
        vfs_write(mgr->pset.stdin,mgr->pset.stdin->length,data,size);
    free(header);
}

pck_t *poll_next(pckmgr *mgr)
{
    pck_t *pck = malloc(sizeof(pck_t));
    while((mgr->pset.stdout->length - mgr->stdout_seeker) < sizeof(pckhead_t));
    vfs_read(mgr->pset.stdout,mgr->stdout_seeker,pck, sizeof(pckhead_t));
    mgr->stdout_seeker += sizeof(pckhead_t);
    if(pck->size > 12)
    {
        int data_size = pck->size-12;
        pck->data = malloc(data_size);
        while((mgr->pset.stdout->length - mgr->stdout_seeker) < data_size);
        vfs_read(mgr->pset.stdout,mgr->stdout_seeker, pck->data,data_size);
        mgr->stdout_seeker += data_size;
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
    iterator_t recieved_pck_it = iterator_create(mgr->recieved_pcks);
    while(!list_is_last(&recieved_pck_it) && !list_is_empty(mgr->recieved_pcks))
    {
        pck_t *current = list_get_current(&recieved_pck_it);
        if(current->id == id)
        {
            list_remove(&recieved_pck_it);
            return current;
        }
        list_next(&recieved_pck_it);
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



pck_t *fetch_pipe(pckmgr *mgr)
{
    if(mgr->pset.stdout->length)
        if((mgr->pset.stdout->length - mgr->stdout_seeker) < sizeof(pckhead_t))
            return NULL;
    return poll_next(mgr);
}

