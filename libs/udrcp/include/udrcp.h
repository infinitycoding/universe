#ifndef _udrcp_h_
#define _udrcp_h_
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

#include <stdint.h>
#include <list.h>
#include <stdio.h>

typedef unsigned int pckid_t;
typedef unsigned int pcktype_t;
#define MAX_ID 0xFFFFFFFF

typedef struct
{
    pckid_t id;
    size_t size;
    pcktype_t type;
}package_t;


typedef struct
{
    unsigned int counter;
    list_t *used_ids;
        //pipes
    int in;
    int out;
    int err;
    list_t *recieved_pcks;
}pckmgr;


pckid_t gen_pckid(pckmgr *mgr);
bool free_pckid(pckmgr *mgr, pckid_t id);

#endif