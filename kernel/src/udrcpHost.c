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


#include <udrcpHost.h>



pckmgr *new_pckmgr(vfs_inode_t *in, vfs_inode_t *out, vfs_inode_t *err)
{
    pckmgr *mgr = malloc(sizeof(pckmgr));
    mgr->counter = 0;
    mgr->used_ids = list_create();
    mgr->p.stdin = in;
    mgr->p.stdout = out;
    mgr->p.stderr = err;
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


bool free_pckid(pckmgr *mgr, pckid_t id)
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

pckid_t send_package(pckmgr *mgr, pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    pckid_t id = gen_pckid(mgr);
    header->id = id;
    header->size = size+12;
    header->type = type;
    vfs_write(mgr->p.stdin,mgr->p.stdin->length,header,sizeof(pckhead_t));
    vfs_write(mgr->p.stdin,mgr->p.stdin->length,data,size);
    free(header);
    return id;
}


void respond(pckmgr *mgr,pckid_t id,pcktype_t type, size_t size, void *data)
{
    pckhead_t *header = malloc(sizeof(pckhead_t));
    header->id = id;
    header->size = size+12;
    header->type = type;
    vfs_write(mgr->p.stdin,mgr->p.stdin->length,header,sizeof(pckhead_t));
    vfs_write(mgr->p.stdin,mgr->p.stdin->length,data,size);
    free(header);
    return id;
}


