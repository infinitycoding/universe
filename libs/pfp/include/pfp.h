#ifndef	_pfp_h_
#define	_pfp_h_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the PFP-library.

     The PFP-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The PFP-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with the PFP-library.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 **/



#include <list.h>
#include <stdint.h>


#define NOTHING 0
#define REPLACE 1
#define APPEND 2
#define UNESSENTIAL  4
#define SERVICE  8
#define DRIVER     16
#define KERNELROOT 32







typedef int ptype;

typedef enum
{
    DEFAULT_MODE = 1,
    SECTION_MODE = 2,
    SECTION_NAME_MODE = 4,
    SECTION_TYPE_MODE = 8,
    SERVICE_MODE = 16,
    SERVICE_NAME_MODE = 32, 
    PIPE_MODE = 64,
    PIPE_NAME_MODE = 128, // pipemode in which names are allowed
    CHILDREN_MODE = 256,
    FALLBACK_MODE = 512,
}parser_mode_t;

struct pnode
{
    char *file;
    ptype type;
    struct pnode *fallback;
    list_t *subtree;
};


struct section
{
    ptype type;
    char *name;
    list_t *subtree;
};

struct parser_mode
{
    parser_mode_t mode;
    struct pnode *node;
};

struct parser_state
{
    struct section *section;    // NULL pointer by defualt
    struct pnode *node;         // NULL pointer by default
    list_t *section_list;       // empty list by default
    parser_mode_t mode;         // DEFAULT_MODE by defualt
    list_t *mode_stack;         // empty list by defualt
    bool success;               //true by default
};


int validate_pf(char *file);

void mode_push(struct parser_state *state);
bool mode_pull(struct parser_state *state);

#endif
