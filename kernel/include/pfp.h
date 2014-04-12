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
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 **/



#include <list.h>
#include <stdint.h>



#define ESSENTIAL  1
#define SERVICE  2
#define KERNELROOT 4

#define STRING_SECTION_LEN 7



typedef unsigned int ptype;



struct ptype_ext
{
    ptype essential;
    ptype service;
    ptype kernelroot;
};


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



list_t *pfp(char *pipeline_file);
bool validate_pf(char *pipelines);
int count_sections(char *pipelines);
struct section parser_section(char *pipelines, char *section);
struct ptype_ext extend_ptype(ptype type);
ptype compress_ptype(struct ptype_ext type);



#endif
