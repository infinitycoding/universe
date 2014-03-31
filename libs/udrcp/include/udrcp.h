#ifndef _udrcp_h_
#define _udrcp_h_
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

#include <stdint.h>
#include <list.h>
#include <stdio.h>

typedef unsigned int pckid_t;
typedef unsigned int pcktype_t;

#define UHOST_DEFAULT_ASYNCHRON "1:00"
#define UHOST_DEFAULT_ASYNCHRON_SIZE 4
#define UHOST_DEFAULT_SYNCHRON  "1:00:S"
#define UHOST_DEFAULT_SYNCHRON_SIZE 6
#define MAX_ID 0xFFFFFFFF

/// Subsystem Request
#define RESET_CON   0
#define DISCONNECT  1
#define PING        2
#define DMA_ALLOC   3
#define DMA_FREE    4
#define INT_REQ     5
#define INT_FREE    6
#define PORT_ALLOC  7
#define PORT_FREE   8
#define VPORT_R     9
#define VPORT_W     10


/// Subsystem Response
#define ERROR       0
#define CONFIRM     1
#define SUCCESS     2


/// Host Request
#define RESET_CON   0
#define DISCONNECT  1
#define SHUTDOWN    2
#define RESTART     3
#define CHKDEV      4

///Host Signals
#define INTSIG      5


/// Host Response
#define ERROR       0
#define CONFIRM     1
#define SUCCESS     2
#define PONG        3


typedef struct
{
    pckid_t id;
    size_t size;
    pcktype_t type;
}pckhead_t;

typedef struct
{
    pckid_t id;
    size_t size;
    pcktype_t type;
    unsigned char *data;
}pck_t;


typedef struct
{
    unsigned int counter;
    list_t *used_ids;
        //pipes
    int in;
    int out;
    int err;
    list_t *recieved_pcks;
    char *version;
    int connection_state;
}pckmgr;


pckmgr *new_pckmgr(int in, int out, int err);
pckid_t gen_pckid(pckmgr *mgr);
void reset_conn(pckmgr *mgr);
int free_pckid(pckmgr *mgr, pckid_t id);
void free_pck(pck_t *package);
pckid_t send_package(pckmgr *mgr, pcktype_t type, size_t size, void *data);
void respond(pckmgr *mgr,pckid_t id,pcktype_t type, size_t size, void *data);
int subsystem_connect(pckmgr *mgr, char *protocol_version);

//todo
pck_t *poll_next(pckmgr *mgr);
void poll_queue(pckmgr *mgr);

pck_t *fetch_queue(pckmgr *mgr,pckid_t id);
pck_t *pck_poll(pckmgr *mgr, pckid_t id);

int free_intsig(pckmgr *mgr, unsigned int num);
int req_intsig(pckmgr *mgr, unsigned int num);


#endif
