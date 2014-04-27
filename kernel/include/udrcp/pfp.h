#ifndef	_PFP_H_
#define	_PFP_H_

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



#define NOTHING 0
#define ESSENTIAL  1
#define SERVICE  2
#define KERNELROOT 4
#define REPLACE 8
#define APPEND 16
#define UNDEFINED 32
#define FAILTURE 64

#define STRING_SECTION_LEN 7
#define STRING_REPLACE_LEN 7
#define STRING_APPEND_LEN 6



typedef unsigned int ptype;



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
struct section *parser_section(char *pipelines, int *section_pos);
int find_next_section(char *pipelines, int *search_begin);
bool is_whitespace(char character);
int skip_whitespaces(char *pipelines, int *start);
int count_sectionname_length(char *pipelines, int *start);
ptype get_section_type(char *pipelines, int *start);
bool check_section_type_given(char *pipelines, int *start);
int skip_until_whitespace(char *pipelines, int *start);



#endif
