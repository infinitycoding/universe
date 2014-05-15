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

#include <list.h>
#include <pmm.h>

#include <sched/thread.h>
#include <sched/process.h>

#include <udrcp/memory.h>


/**
 * @brief Handles a PMA_ALLOC request
 * @param drv pointer to the driver struct
 * @param req pointer to the request package
 */
void handle_pma_alloc(struct driver *drv, pck_t *req)
{
    struct pma_request *pma_req = (struct pma_request*) req->data;
    paddr_t phys_base;

    switch(pma_req->type)
    {
        case PMA_ALLOC_RANDOM:
            phys_base = pmm_alloc_page_range(pma_req->size/PAGE_SIZE);
        break;


        case PMA_ALLOC_SPECIFIC:
           /*
           TODO: Implement HW resource check or allocate free PMM memory 
            if(!pmm_check_page_range(pma_req->phys_base, pma_req->range))
            {
                respond(drv->pman, req->id, ERROR, 0, NULL);
                return;
            }*/

            phys_base = pma_req->phys_base;
            //pmm_mark_page_range_as_used(phys_base, pma_req->size/PAGE_SIZE);
        break;

        case PMA_ALLOC_LOWER:
            phys_base = pmm_alloc_page_limit(pma_req->phys_base);
        break;

        default:
            respond(drv->pman, req->id, ERROR, 0, NULL);
            return;
         break;
     }


    iterator_t i = iterator_create(drv->process->threads);
    struct pma_response resp;
    resp.mem_base = vmm_automap_user_range( &((struct thread_state*)list_get_current(&i))->context, pma_req->phys_base, pma_req->size/PAGE_SIZE, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
    resp.phys_base = pma_req->phys_base;
    printf("memory available!\n");
    respond(drv->pman, req->id, SUCCESS, sizeof(struct pma_response), &resp);
}


/**
 * @brief Handles a PMA_FREE request
 * @param drv pointer to the driver struct
 * @param req pointer to the request package
 */
void handle_pma_free(struct driver *drv, pck_t *req)
{
     /*TODO: free in HW resource list or free the page in PMM*/

}