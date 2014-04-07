#ifndef	_pfp_h_
#define	_pfp_h_

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

#include <list.h>

typedef enum
{
    essential = 1,
    optional = 0
}ess_t;


struct pnode
{
    char *file;
    list_t *subtree;
};

struct section
{
    ess_t state;
    struct pnode *service;
    struct pnode *pipeline;
};

struct pipe_tree
{
    struct section *boot;
    struct section *common;
    struct section *fallback;
};


char *get_section(const char *p, const char *section);
struct pipe_tree *pfp(char *p);
struct pnode *parser_sub_section(const char *p, const char *section);

#endif
