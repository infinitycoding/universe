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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <pfp.h>


char *comment_handle(char *str, struct parser_state *state);
char *section_handle(char *str, struct parser_state *state);
char *kernel_root_handle(char *str, struct parser_state *state);
char *pipe_handle(char *str, struct parser_state *state);
char *semicolon_handle(char *str, struct parser_state *state);
char *colon_handle(char *str, struct parser_state *state);
char *service_handle(char *str, struct parser_state *state);
char *open_square_brace_handle(char *str, struct parser_state *state);
char *comma_handle(char *str, struct parser_state *state);
char *close_square_brace_handle(char *str, struct parser_state *state);
char *fallback_handle(char *str, struct parser_state *state);
char *end_fallback_handle(char *str, struct parser_state *state);
char *section_type_handle(char *str, struct parser_state *state);
char *end_section_type_handle(char *str, struct parser_state *state);


char *identifiers[] = {"#", "section", "service", "->", "|", "[", "]", "~{", "}", ";", ":", "," ,"(", ")", NULL};
char *(*identifier_handles[])(char *str, struct parser_state *state) =
{
    comment_handle, section_handle, service_handle, kernel_root_handle, pipe_handle,
    open_square_brace_handle, close_square_brace_handle, fallback_handle, end_fallback_handle, semicolon_handle,
    colon_handle, comma_handle, section_type_handle, end_section_type_handle, NULL
};


/**
 * @brief Checks if str is a identifier
 * @param str String to be checked
 * @return numer of the identifier in the identifiers array. If it's not an identifier the function returns -1
 */
int is_identifier(char *str)
{
    int i;
    size_t str_len = strlen(str);
    for(i = 0; identifiers[i] != NULL; i++)
    {
        size_t id_len = strlen(identifiers[i]);
        if(id_len > str_len)
            continue;
        if(strncmp(str,identifiers[i],id_len) == 0)
            break;
    }

    if(identifiers[i] == NULL)
    {
        return -1;
    }

    return i;
}


/**
 * @brief Calls an identifier handle from the handle array
 * @param num Handle number
 * @param str current string to be parserd
 * @param state the current parser state
 */
char *call_identifier_handle(int num, char *str, struct parser_state *state)
{
    str = str+strlen(identifiers[num]);

    if(identifier_handles[num] == NULL)
    {
        printf("Identifier %s\n", identifiers[num]);
        return str;
    }
    return identifier_handles[num](str,state);
}


/**
 * @brief Skips comments in the file to be parserd
 * @param str the comment to be skipped
 * @param state the parsing state
 * @return A pointer to the first character after the comment
 */
char *comment_handle(char *str, struct parser_state *state)
{
    size_t i = 0;
    while((str[i] != '\n') && (i < strlen(str)))
        i++;
    printf("comment lengh: %d\n",i);
    return str+i;
}


char *section_handle(char *str, struct parser_state *state)
{
    // check mode
    if(! (state->mode & (DEFAULT_MODE|SECTION_MODE)))
    {
        printf("invalid identifier section\n");
        state->success = false;
        return str;
    }

    state->section = malloc(sizeof(struct section));
    state->section->type = APPEND;
    state->section->name = NULL;
    state->section->subtree = NULL;
    list_push_front(state->section_list, state->section);

    state->node = NULL;
    state->mode = SECTION_NAME_MODE;

    printf("Section ");
    return str;
}

char *service_handle(char *str, struct parser_state *state)
{
    // check mode
    if(! (state->mode & SECTION_MODE))
    {
        printf("invalid identifier \"service\"\n");
        state->success = false;
        return str;
    }

    mode_push(state);

    struct pnode *new_node = malloc(sizeof(struct pnode));
    new_node->type = SERVICE;
    new_node->file = NULL;
    new_node->fallback = NULL;
    new_node->subtree = NULL;

    state->mode = SERVICE_NAME_MODE;
    state->node = new_node;

    if(!state->section->subtree)
        state->section->subtree = list_create();
    list_push_front(state->section->subtree, new_node);


    printf("Service: ");

    return str;
}


char *kernel_root_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (SECTION_MODE | SERVICE_MODE | PIPE_MODE | PIPE_NAME_MODE)))
    {
        printf("no section specified before first pipe (->)\n");
        state->success = false;
        return str;
    }

    printf("Kernel Root Nodes:\n");
    mode_push(state);


    struct pnode *new_node = malloc(sizeof(struct pnode));
    state->mode = PIPE_NAME_MODE;
    new_node->type = KERNELROOT;
    new_node->file = NULL;
    new_node->fallback = NULL;

    if(!state->node || state->mode == SECTION_MODE)
    {
        if(!state->section->subtree)
            state->section->subtree = list_create();
        list_push_front(state->section->subtree, new_node);
    }
    else
    {
        if(!state->node->subtree)
            state->node->subtree = list_create();
        list_push_front(state->node->subtree, new_node);
    }

    state->node = new_node;
    return str;
}


char *pipe_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & PIPE_MODE))
    {
        printf("no section or driver tree specified before first pipe (|) %d\n",state->mode);
        state->success = false;
        return str;
    }

    printf("Pipe\n");
    struct pnode *new_node = malloc(sizeof(struct pnode));
    new_node->type = NOTHING;
    new_node->file = NULL;
    new_node->fallback = NULL;
    new_node->subtree = NULL;

    if(!state->node->subtree)
            state->node->subtree = list_create();
    list_push_front(state->node->subtree, new_node);

    state->node = new_node;
    state->mode = PIPE_NAME_MODE;
    return str;

}


char *open_square_brace_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (PIPE_NAME_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \"[\"\n");
        state->success = false;
        return str;
    }

    state->mode = CHILDREN_MODE;
    mode_push(state);

    struct pnode *new_node = malloc(sizeof(struct pnode));
    new_node->type = NOTHING;
    new_node->file = NULL;
    new_node->fallback = NULL;
    new_node->subtree = NULL;

    if(!state->node->subtree)
            state->node->subtree = list_create();
    list_push_front(state->node->subtree, new_node);

    state->node = new_node;
    state->mode = PIPE_NAME_MODE;
    return str;
}


char *close_square_brace_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (PIPE_NAME_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \"[\"\n");
        state->success = false;
        return str;
    }

    if(!mode_pull(state))
    {
        state->success = false;
        return str;
    }

    if(state->mode != CHILDREN_MODE)
    {
        printf("invalid identifier \"]\"\n");
        state->success = false;
        return str;
    }

    state->mode = PIPE_MODE;
    return str;
}


char *colon_handle(char *str, struct parser_state *state)
{
    if(! (state->mode &  (SERVICE_NAME_MODE | SECTION_NAME_MODE)))
    {
        printf("invalid identifier \":\"\n");
        state->success = false;
        return str;
    }

    if(state->mode & SERVICE_NAME_MODE)
        state->mode = SERVICE_MODE;
    else
    {
        if(state->section->name == NULL)
        {
            printf("missing section name\n");
            state->success = false;
            return str;  
        }

        state->mode = SECTION_MODE;
    }
    return str;
}

char *semicolon_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (SERVICE_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \";\"\n");
        state->success = false;
        return str;
    }
    if(state->mode & PIPE_MODE)
    {
        if(!mode_pull(state))
        {
            state->success = false;
            return str;
        }

        if(state->mode & SERVICE_MODE)
        {
            if(!mode_pull(state))
            {
                state->success = false;
                return str;
            }
        }
    }
    else
    {
        if(!mode_pull(state))
        {
            state->success = false;
            return str;
        }
    }
    state->node = NULL;
    return str;
}


char *comma_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & PIPE_MODE ))
    {
        printf("invalid identifier \",\"\n");
        state->success = false;
        return str;
    }


    if(!mode_pull(state))
    {
        state->success = false;
        return str;
    }

    if(state->mode != CHILDREN_MODE)
    {
        printf("invalid identifier \",\"\n");
        state->success = false;
        return str;
    }

    mode_push(state);
    state->mode = PIPE_NAME_MODE;
    return str;
}


char *fallback_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & PIPE_MODE ))
    {
        printf("invalid identifier \"~{\"\n");
        state->success = false;
        return str;
    }

    state->mode = FALLBACK_MODE;
    mode_push(state);

    struct pnode *new_node = malloc(sizeof(struct pnode));
    new_node->type = NOTHING;
    new_node->file = NULL;
    new_node->fallback = NULL;
    new_node->subtree = NULL;

    state->node->fallback = new_node;
    state->node = new_node;
    state->mode = PIPE_NAME_MODE;
    return str;
}



char *end_fallback_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & (PIPE_NAME_MODE | PIPE_MODE) ))
    {
        printf("invalid identifier \"}\"\n");
        state->success = false;
        return str;
    }

    if(!mode_pull(state))
    {
        state->success = false;
        return str;
    }

    if(state->mode != FALLBACK_MODE)
    {
        printf("invalid identifier \"}\"\n");
        state->success = false;
        return str;
    }

    state->mode = PIPE_MODE;
    return str;
}



char *section_type_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & SECTION_NAME_MODE ))
    {
        printf("invalid identifier \"(\" \n");
        state->success = false;
        return str;
    }

    state->mode = SECTION_TYPE_MODE;
    return str;
}

char *end_section_type_handle(char *str, struct parser_state *state)
{
    if(! (state->mode & SECTION_TYPE_MODE ))
    {
        printf("invalid identifier \")\"\n");
        state->success = false;
        return str;
    }


    state->mode = SECTION_NAME_MODE;
    return str;
}