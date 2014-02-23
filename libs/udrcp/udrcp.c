#include <udrcp.h>
#include <stdlib.h>

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

pckid_t mgr_send_package(pckmgr *mgr, pcktype_t type, size_t size, void *data)
{
    package_t *header = malloc(sizeof(package_t));
    pckid_t id = gen_pckid(mgr);
    header->id = id;
    header->size = size;
    header->type = type;
    write(mgr->out,header,sizeof(package_t));
    free(header);
    write(mgr->out,data,size);
    return id;
}






