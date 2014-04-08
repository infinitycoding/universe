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


 #include <string.h>
 #include <heap.h>
 #include <pfp.h>

/**
 * pipline file parser (pfp)
 * @param 0 pipeline file content
 * @return pipeline ram struct
 */
struct pipe_tree *pfp(char *p)
{
    struct pipe_tree *tree = malloc(sizeof(struct pipe_tree));
    tree->boot = malloc(sizeof(struct section));
    char *boot = get_section(p,"boot");
    if(boot)
    {
        tree->boot->service = parser_sub_section(boot,"service");
        tree->boot->pipeline = parser_sub_section(boot,"pipeline");
    }
    else
    {
        tree->boot->service = NULL;
        tree->boot->pipeline = NULL;
    }


    tree->common = malloc(sizeof(struct section));
    char *common = get_section(p,"common");
    if(common)
    {
        tree->common->service = parser_sub_section(common,"service");
        tree->common->pipeline = parser_sub_section(common,"pipeline");
    }
    else
    {
        tree->common->service = NULL;
        tree->common->pipeline = NULL;
    }

    tree->fallback = malloc(sizeof(struct section));
    char *fallback = get_section(p,"boot");
    if(fallback)
    {
        tree->fallback->service = parser_sub_section(fallback,"service");
        tree->fallback->pipeline = parser_sub_section(fallback,"pipeline");
    }
    else
    {
        tree->fallback->service = NULL;
        tree->fallback->pipeline = NULL;
    }
    return tree;
}


char *get_section(const char *p, const char *section)
{
    int plen = strlen(p);
    int slen = strlen(section);
    int i;
    for(i = 0; i < plen-slen; i++)
    {
        if((strncmp(p+i,"section ",8) == 0 || strncmp(p+i,"SECTION ",8) == 0) && strncmp(p+i+8,section,slen) == 0)
        {
            int j;
            char *r;
            for(j = i; j < plen-8; j++)
            {
                if(strncmp(p+j,"section ",8) == 0)
                {
                    r = malloc((j-i)+1);
                    r[j-i] = 0;
                    strncpy(r,p+i,j-i);
                    return r;
                }
            }

            r = malloc((plen-i)+1);
            r[plen-i] = 0;
            strncpy(r,p+i,plen);
            return r;
        }
    }
    return NULL;
}


struct pnode *parser_sub_section(const char *p, const char *section)
{
    //TODO...
    return NULL;
}
