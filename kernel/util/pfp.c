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

        if(sections_nr == 0)
            return NULL;

        int i = 0;
        int current_position = 0;

        for(i = 0; i < sections_nr; i++)
        {
            struct section *current_section = parser_section(pipelines, &current_position);
            list_push_back(pipes, current_section);
        }

        return pipes;
    }
    else
    {
        return NULL;
    }
}


bool validate_pf(char *pipelines)
{
    // TODO

    return FALSE;
}


int count_sections(char *pipelines)
{
    int i = 0;
    int nr = 0;

    for(i = 0; pipelines[i + STRING_SECTION_LEN] != '\0'; i++)
    {
        if(pipelines[i] == '#')
            for(; pipelines[i] != '\n'; i++);

        if(strncmp(&pipelines[i], "section", STRING_SECTION_LEN) == 0)
            nr++;
    }
        

    return nr;
}


struct section *parser_section(char *pipelines, int *section_pos)
{
    find_next_section(pipelines, section_pos);

    (*section_pos) += 7;

    skip_whitespaces(pipelines, section_pos);

    int section_name_length = count_sectionname_length(pipelines, section_pos);

    struct section *this = (struct section *)malloc(sizeof(struct section));
    this->name = (char *)malloc(sizeof(char) * (section_name_length + 1));
    memset(this->name, '\0',sizeof(char) * (section_name_length + 1));
    strncpy(this->name, &pipelines[(*section_pos)], section_name_length);
    (*section_pos) += section_name_length;
    this->type = get_section_type(pipelines, section_pos);

    // TODO

    return this;
}


int find_next_section(char *pipelines, int *search_begin)
{
    for(; pipelines[(*search_begin) + STRING_SECTION_LEN] != '\0'; (*search_begin)++)
    {
        if(pipelines[(*search_begin)] == '#')
            for(; pipelines[(*search_begin)] != '\n'; (*search_begin)++);

        if(strncmp(&pipelines[(*search_begin)], "section", STRING_SECTION_LEN) == 0)
            break;
    }
        

    return (*search_begin);
}


bool is_whitespace(char character)
{
    return ((character == ' ' || character == '\t' || character == '\n') ? true : false);
}


int skip_whitespaces(char *pipelines, int *start)
{
    for(; is_whitespace(pipelines[(*start)]); (*start)++);

    return (*start);
}


int count_sectionname_length(char *pipelines, int *start)
{
    int i = (*start);
    int nr = 0;

    for(nr = 0; !is_whitespace(pipelines[i]); i++)
        nr++;

    return nr;
}


ptype get_section_type(char *pipelines, int *start)
{
    ptype t = NOTHING;

    if(check_section_type_given(pipelines, start) == FALSE)
        return UNDEFINED;

    skip_whitespaces(pipelines, start);
    (*start)++;

    if(strncmp(&pipelines[(*start)], "replace", STRING_REPLACE_LEN) == 0)
    {
        t |= REPLACE;
        (*start) += (STRING_REPLACE_LEN + 1);
    }
    else if(strncmp(&pipelines[(*start)], "append", STRING_APPEND_LEN) == 0)
    {
        t |= APPEND;
        (*start) += (STRING_APPEND_LEN + 1);
    }
    else
    {
        t |= FAILTURE;
        skip_until_whitespace(pipelines, start);
    }

    return t;
}


bool check_section_type_given(char *pipelines, int *start)
{
    int i = (*start);

    for(; pipelines[i] != ':'; i++)
        if(pipelines[i] == '(')
            return TRUE;

    return FALSE;

}


int skip_until_whitespace(char *pipelines, int *start)
{
    for(; !is_whitespace(pipelines[(*start)]); (*start)++);

    return (*start);
}
