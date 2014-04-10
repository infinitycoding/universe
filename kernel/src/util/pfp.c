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


 #include <string.h>
 #include <heap.h>
 #include <pfp.h>



list_t *pfp(char *pipelines)
{
    bool valid = validate_pf(pipelines);

    if(valid == true)
    {
        list_t *pipes = list_create();
        int sections_nr = count_sections(pipelines);

        return pipes;
    }
    else
    {
        return NULL;
    }
}


bool validate_pf(char *pipelines)
{

}


int count_sections(char *pipelines)
{
    int i = 0;
    int nr = 0;

    for(i = 0; pipelines[i + STRING_SECTION_LEN] != '\0'; i++)
        if(strncmp(pipelines[i], "section", STRING_SECTION_LEN) == 0)
            nr++;

    return nr;
}


struct section parser_section(char *pipelines, char *section)
{

}


struct ptype_ext extend_ptype(ptype type)
{
    struct ptype_ext result;

    result.essential = type & ESSENTIAL;
    result.service = type & SERVICE;
    result.kernelroot = type & KERNELROOT;

    return result;
}


ptype compress_ptype(struct ptype_ext type)
{
    ptype result = 0;

    result |= type.essential;
    result |= type.service;
    result |= type.kernelroot;

    return result;
}