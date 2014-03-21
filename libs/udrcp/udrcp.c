#include <udrcp.h>
#include <stdlib.h>
#include <string.h>


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
    write(mgr->out,header,sizeof(pckhead_t));
    free(header);
    write(mgr->out,data,size);
    return id;
}

bool subsystem_connect(pckmgr *mgr, char *protocol_version)
{
    send_package(mgr, RESET_CON, 0, NULL);
    pckid_t ping_id = send_package(mgr, PING, strlen(protocol_version), protocol_version);
    pckhead_t *pck_header = malloc(sizeof(pckhead_t));
    read(mgr->in,pck_header,sizeof(pckhead_t));
    if(pck_header->id == ping_id && pck_header->type == PONG)
    {
        mgr->version = malloc(pck_header->size+1);
        mgr->version[pck_header->size] = 0;
        read(mgr->in,mgr->version,pck_header->size-sizeof(pckhead_t));
    }
    else
    {
        write(mgr->err,"invalind ping response from host",32);
        return false;
    }
    //todo: check udrcp version
    return true;
}


pck_t *poll_next(pckmgr *mgr)
{
    pck_t *pck = malloc(sizeof(pck_t));
    read(mgr->in,pck,sizeof(pckhead_t));
    if(pck->size > 12)
    {
        int data_size = pck->size-12;
        pck->data = malloc(data_size);
        read(mgr->in,pck->data,data_size);
    }
    else
        pck->data = NULL;
    return pck;
}


void poll_queue(pckmgr *mgr)
{
    list_push_front(mgr->recieved_pcks,poll_next(mgr));
}

pck_t *pck_fetch(pckmgr *mgr,pckid_t id)
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
    pck_t *pck = pck_fetch(mgr,id);
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











