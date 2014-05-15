/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the UDRCP-library.
 
     The UDRCP-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The UDRCP-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the UDRCP-library.  If not, see <http://www.gnu.org/licenses/>.
 */



/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 */


 #include <memory.h>
 #include <stdlib.h>


/**
 *  @brief Requests a shared memory from the connection partner.
 *  @param mgr The pakage manager of the connection
 *  @param size the size of the shared area
 *  @return pointer to a shm_mem struct or NULL (failure)
 */
 shm_mem_t *alloc_shm(pckmgr *mgr, size_t size)
 {
    pckid_t id = send_package(mgr, SHM_ALLOC, sizeof(size_t), &size);
    while(1)
    {
        pck_t *response = pck_poll(mgr,id);
        switch(response->type)
        {
             case ERROR:
                 free(response->data);
                 free(response);
                 return (shm_mem_t *)NULL;
             break;
 
             case CONFIRM:
                 free(response);
             break;
 
             case SUCCESS:
                 if(response->size != sizeof(void *))
                 {
                     udrcp_error(mgr,"error: invalid response size. Expected sizeof(shm_mem_t):%d. Got package:%d type:%d size:%d\n ",sizeof(shm_mem_t),response->id,response->type,response->size);
                     exit(1);
                 }
                 shm_mem_t *mem = malloc(sizeof(shm_mem_t));
                 mem->mgr = mgr;
                 mem->mem_base = response->data;
                 mem->range = size;
                 free(response);
                 return mem;
             break;

             default:
                 udrcp_error(mgr,"error: unsupported response\n");
                 exit(1);
             break;
        };
    }

    return NULL;
 }


 /**
 * @brief Polls the response from the comunication partner for memory requests
 * @param mgr The pakage manager of the connection
 * @param id id of the request package
 */
static pma_mem_t *poll_pma_response(pckmgr *mgr, size_t size, pckid_t id)
 {
    while(1)
    {
        pck_t *response = pck_poll(mgr,id);
        switch(response->type)
        {
             case ERROR:
                 free(response->data);
                 free(response);
                 return (pma_mem_t *)NULL;
             break;
 
             case CONFIRM:
                 free(response);
             break;
 
             case SUCCESS:
                 if(response->size != sizeof(struct pma_response))
                 {
                     udrcp_error(mgr,"error: invalid response size. Expected sizeof(pma_mem_t):%d. Got package:%d type:%d size:%d\n ",sizeof(pma_mem_t),response->id,response->type,response->size);
                     exit(1);
                 }

                 struct pma_response *resp = (struct pma_response*)response->data;
                 pma_mem_t *mem = malloc(sizeof(pma_mem_t));
                 mem->mem_base = (void *)resp->mem_base;
                 mem->phys_base = (void *)resp->phys_base;
                 mem->mgr = mgr;
                 mem->range = size;
                 free(response->data);
                 free(response);
                 return mem;
             break;

             default:
                 udrcp_error(mgr,"error: unsupported response\n");
                 exit(1);
             break;
        };
    }

    return NULL;
 }


/**
 *  @brief Requests a data area with it's physical adress.
 *  @param mgr The pakage manager of the connection to the parent
 *  @param size the size of the area
 *  @return pointer to a pma_mem struct or NULL (failure)
 */
pma_mem_t *alloc_pma(pckmgr *mgr, size_t size)
{
    struct pma_request req;
    req.type = PMA_ALLOC_RANDOM;
    req.size = size;

    pckid_t id = send_package(mgr, PMA_ALLOC, sizeof(struct pma_request), &req);
    return poll_pma_response(mgr, size, id);
}


/**
 *  @brief Requests a data area with a specific physical adress.
 *  @param mgr The pakage manager of the connection to the parent
 *  @param size the size of the area
 *  @return pointer to a pma_mem struct or NULL (failure)
 */
pma_mem_t *alloc_pma_area(pckmgr *mgr, void *phys_base, size_t size)
{
    struct pma_request req;
    req.type = PMA_ALLOC_SPECIFIC;
    req.size = size;
    req.phys_base = (paddr_t)phys_base;

    pckid_t id = send_package(mgr, PMA_ALLOC, sizeof(struct pma_request), &req);
    return poll_pma_response(mgr, size, id);
}


/**
 *  @brief Requests a data area below phys_limit.
 *  @param mgr The pakage manager of the connection to the parent
 *  @param size the size of the area
 *  @return pointer to a pma_mem struct or NULL (failure)
 */
pma_mem_t *alloc_pma_lower_area(pckmgr *mgr, void *phys_limit, size_t size)
{
    struct pma_request req;
    req.type = PMA_ALLOC_LOWER;
    req.size = size;
    req.phys_base = (paddr_t)phys_limit;

    pckid_t id = send_package(mgr, PMA_ALLOC, sizeof(struct pma_request), &req);
    return poll_pma_response(mgr, size, id);
}


/**
 * @breif Frees allocated shared memory.
 * @param mem memory struckt
 * @return 1=sucess, 0= failure
 */
int free_shm(shm_mem_t *mem)
{
    pckid_t id = send_package(mem->mgr,SHM_FREE,sizeof(void *),&mem->mem_base);
    free(mem);
    while(1)
    {
        pck_t *response = pck_poll(mem->mgr,id);
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

/**
 * @breif Frees allocated pyhsical memory access area.
 * @param mem memory struckt
 * @return 1 = sucess, 0 = failure
 */
int free_pma(pma_mem_t *mem)
{
    pckid_t id = send_package(mem->mgr,PMA_FREE,sizeof(void *),&mem->mem_base);
    free(mem);
    while(1)
    {
        pck_t *response = pck_poll(mem->mgr,id);
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



