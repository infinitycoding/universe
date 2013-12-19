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

typedef unsigned int pckid_t;

//default Request types (driver->host)
typedef enum
{
    REQ_DEV_LST = 0,
    REQ_DEV_USE = 1,
    REQ_TRIGGER = 2,
    REQ_SHM = 3,
    // the rest can be used to specify host specific requests
}req_t;

typedef enum
{
    SUCESS = 1,
    FAILURE = 0
}resp_t;


// default responses (host->driver)

typedef struct
{
    pckid_t id;
    resp_t sucess; // bit[0]: resp_t; rest: if bit[0] == FAILURE -> error code, if bit[0] == SUCESS -> size of response package
}default_response;





#endif
