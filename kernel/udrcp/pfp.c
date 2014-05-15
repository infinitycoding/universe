/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Universe Kernel.

     The Universe Kernel is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The Universe Kernel is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with the Universe Kernel. If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 */


#include <stdint.h>
#include <string.h>
#include <sys/multiboot.h>
#include <mm/heap.h>
#include <udrcp/pfp.h>



/**
 * @brief parsers a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @return a list of the sections
 */
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


/**
 * @brief validates a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @return true if the pipeline file is valid
 * @return false if the pipeline file is invalid
 */
bool validate_pf(char *pipelines)
{
    // TODO

    return true;
}


/**
 * @brief counts the number of sections in a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @return the number of sections
 */
int count_sections(char *pipelines)
{
    int i = 0;
    int nr = 0;

    for(i = 0; pipelines[i + STRING_SECTION_LEN] != '\0'; i++)
    {
        if(pipelines[i] == '#')
            for(; pipelines[i] != '\n'; i++);

        if(!strncmp(&pipelines[i], "section", STRING_SECTION_LEN))
            nr++;
    }


    return nr;
}


/**
 * @brief parsers a section of a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @param section_pos the position in the string where the section starts
 */
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
    skip_until_colon(pipelines, section_pos);
    int section_end = find_end_of_section(pipelines, section_pos);
    this->subtree = list_create();

    for(; (*section_pos) < section_end; (*section_pos)++)
    {
        if(pipelines[(*section_pos)] == '#')
        {
            while(pipelines[(*section_pos)] != '\n' || pipelines[(*section_pos)] != '\0' || pipelines[(*section_pos)] >= section_end)
                    (*section_pos)++;
        }
        else if(pipelines[(*section_pos)] == '<')
        {
            struct pnode *node = parser_pnode(pipelines, section_pos, this->subtree);

            list_push_front(this->subtree, node);
        }
    }

    return this;
}


/**
 * @brief finds the next section in a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @param search_begin the point in the string where i should start searching
 * @return the position of the next section
 */
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


/**
 * @brief finds the end of a section in a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @param search_begin the point in the string where i should start searching
 * @return the point of the end of the current section
 */
int find_end_of_section(char *pipelines, int *search_begin)
{
    int i;

    for(i = (*search_begin); pipelines[i + STRING_SECTION_LEN] != '\0'; i++)
    {
        if(pipelines[i] == '#')
            for(; pipelines[i] != '\n'; i++);

        if(strncmp(&pipelines[i], "section", STRING_SECTION_LEN) == 0)
            break;
    }

    return i;
}


/**
 * @brief checks if a character is a whitespace
 * @param character the character
 * @return true or false
 */
bool is_whitespace(char character)
{
    return ((character == ' ' || character == '\t' || character == '\n') ? true : false);
}


/**
 * @brief skips whitespaces in a string
 * @param pipelines the string
 * @param start the current position in the string
 * @return the new position in the string
 */
int skip_whitespaces(char *pipelines, int *start)
{
    for(; is_whitespace(pipelines[(*start)]); (*start)++);

    return (*start);
}


/**
 * @brief counts the length of the name of a section
 * @param pipelines the string in which the section name is
 * @param start the position of the section name
 * @return the length of the sections name
 */
int count_sectionname_length(char *pipelines, int *start)
{
    int i = (*start);
    int nr = 0;

    for(nr = 0; !is_whitespace(pipelines[i]); i++)
        nr++;

    return nr;
}


/**
 * @brief looks up the type of a section
 * @param pipelines the string in which i should look up the section type
 * @param start the position of the section type definition
 * @return the type of the section
 */
ptype get_section_type(char *pipelines, int *start)
{
    ptype t = NOTHING;

    if(check_section_type_given(pipelines, start) == false)
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


/**
 * @brief checks if there is a section type given
 * @param pipelines the string in which the section type could be
 * @param start the position where i should look for a section type
 * @return true or false
 */
bool check_section_type_given(char *pipelines, int *start)
{
    int i = (*start);

    for(; pipelines[i] != ':'; i++)
        if(pipelines[i] == '(')
            return true;

    return false;

}


/**
 * @brief skips all characters in a string until the next whitespace
 * @param pipelines the string
 * @param start the current position
 * @return the position after skipping the characters
 */
int skip_until_whitespace(char *pipelines, int *start)
{
    for(; !is_whitespace(pipelines[(*start)]); (*start)++);

    return (*start);
}


/**
 * @brief skips all characters in a string until the next colon
 * @param pipelines the string
 * @param start the current position
 * @return the position after skipping the characters
 */
int skip_until_colon(char *pipelines, int *start)
{
    for(; pipelines[(*start)] != ':' && pipelines[(*start)] != '\0'; (*start)++);

    return (*start);
}


/**
 * @brief parsers a pnode in a pipeline file
 * @param pipelines the pipeline file as a ascii string
 * @param start the position of the pnode
 * @param other a list of all pnodes until here (for services)
 * @return a pointer to the pnode struct
 */
struct pnode *parser_pnode(char *pipelines, int *start, list_t *other)
{
    struct pnode *node = (struct pnode *)malloc(sizeof(struct pnode));
    int pnode_end = find_pnode_end(pipelines, start);
    node->type = get_ptype(pipelines, (*start), pnode_end);
    node->file = get_pnode_filename(pipelines, (*start), pnode_end);
    node->subtree = list_create();

    (*start) = pnode_end;

    for(; pipelines[(*start)] != ';'; (*start)++)
    {
        if(pipelines[(*start)] == '|')
        {
            for(; pipelines[(*start)] != '<' && pipelines[(*start)] != '['; (*start)++);

            if(pipelines[(*start)] == '<')
            {
                struct pnode *subnode = parser_pnode(pipelines, start, other);
                list_push_front(node->subtree, subnode);

                //if(pipelines[(*start)] == ';')
            }
            else
            {
                for(; pipelines[(*start)] != ']'; (*start)++)
                {
                    if(pipelines[(*start)] == '<')
                    {
                        struct pnode *subnode = parser_pnode(pipelines, start, other);
                        list_push_front(node->subtree, subnode);
                    }
                }
            }
        }
        else if(pipelines[(*start)] == '~')
        {
            for(; pipelines[(*start)] != '<' && pipelines[(*start)] != '{'; (*start)++);

            if(pipelines[(*start)] == '<')
            {
                struct pnode *subnode = parser_pnode(pipelines, start, other);
                node->fallback = subnode;
            }
            else
            {
                struct pnode *currentnode = node;

                for(; pipelines[(*start)] != '}'; (*start)++)
                {
                    if(pipelines[(*start)] == '<')
                    {
                        struct pnode *subnode = parser_pnode(pipelines, start, other);
                        currentnode->fallback = subnode;
                        currentnode = subnode;
                    }
                }
            }
        }
    }

    return node;
}


/**
 * @brief searches the end of a pnode
 * @param pipelines the string in which the pnode is located
 * @param start the current position
 * @return the end of the pnode
 */
int find_pnode_end(char *pipelines, int *start)
{
    int i;

    for(i = (*start); pipelines[i] != '\0'; i++)
    {
        if(pipelines[i] == '#')
            for(; pipelines[i] != '\n'; i++);

        if(pipelines[i-1] != '-' && pipelines[i] == '>')
            break;
    }

    return i;
}


/**
 * @brief checks if a pnode is a service
 * @param pipelines the string with the pnod in it
 * @param start the start of the pnode
 * @param end the end of the pnode
 * @return true or false
 */
bool is_service(char *pipelines, int start, int end)
{
    int i;

    for(i = start; i < (end - STRING_SERVICE_LEN); i++)
        if(strncmp(&pipelines[i], "service", STRING_SERVICE_LEN) == 0)
            return true;

    return false;
}


/**
 * @brief checks if a pnode is kernelroot
 * @param pipelines the string with the pnod in it
 * @param start the start of the pnode
 * @param end the end of the pnode
 * @return true or false
 */
bool is_kernelroot(char *pipelines, int start, int end)
{
    int i;

    for(i = start; i < (end - ARROW_LEN); i++)
        if(strncmp(&pipelines[i], "->", ARROW_LEN) == 0)
            return true;

    return false;
}


/**
 * @brief returns the type of a pnode
 * @param pipelines the string with the pnod in it
 * @param start the start of the pnode
 * @param end the end of the pnode
 * @return the type of a pnode
 */
ptype get_ptype(char *pipelines, int start, int end)
{
    ptype result = NOTHING;

    if(is_service(pipelines, start, end))
        result |= SERVICE;

    if(is_kernelroot(pipelines, start, end))
        result |= KERNELROOT;

    return result;
}


/**
 * @brief returns the name of a pnode
 * @param pipelines the string with the pnod in it
 * @param start the start of the pnode
 * @param end the end of the pnode
 * @return the name of a pnode
 */
char *get_pnode_filename(char *pipelines, int start, int end)
{
    int i;

    for(i = start; pipelines[i] != '>'; i++)
    {
        if(!strncmp(&pipelines[i], "->", ARROW_LEN))
            i += ARROW_LEN;
        else if(!strncmp(&pipelines[i], "service", STRING_SERVICE_LEN))
            i += STRING_SERVICE_LEN;
        else if(!is_whitespace(pipelines[i]) && pipelines[i] != '<')
        {
            int len = end - i;
            char *name = (char *)malloc(sizeof(char) * (len + 1));
            strncpy(name, &pipelines[i], len);
            return name;
        }
    }

    return NULL;
}


/**
* @brief Get module Data Block
* @param mb_info multiboot struct
* @param name module name
* @return moduel information
*/
struct mods_add* find_module(struct multiboot_struct *mb_info, char *name)
{
    int i;
    while(*name == ' ' || *name == '\t')
        name++;
    int n = 0;
    while(name[n] != ' ' && name[n] != '\0')

        n++;

    struct mods_add* modules = (struct mods_add*) mb_info->mods_addr;
    for(i = 0; i < mb_info->mods_count; i++)
     {
        if(! strncmp(name, modules[i].string, n))
        {
            return &modules[i];
        }

    }
    return NULL;
}
