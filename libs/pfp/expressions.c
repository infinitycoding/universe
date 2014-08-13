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



bool is_name(char *in);
bool is_whitespace(char *str);

char *whitespace_handle(char *str, struct parser_state *state);
char *name_handle(char *str, struct parser_state *state);


bool (*expression_checks[])(char *str) = {is_name, is_whitespace, NULL};
char *(*expression_handles[])(char *str, struct parser_state *state) = {name_handle, whitespace_handle, NULL};


int is_expression(char *str)
{
    int i;
    for(i = 0; expression_checks[i] != NULL; i++)
    {
        if(expression_checks[i](str))
            break;
    }

    if(expression_checks[i] == NULL)
    {
        return -1;
    }

    return i;
}

char *call_expression_handle(int num, char *str, struct parser_state *state)
{
    if(expression_handles[num] == NULL)
    {
        printf("Expression type %d\n", num);
        return str;
    }

    return expression_handles[num](str, state);
}

bool is_whitespace(char *str)
{
    return ((*str == ' ' || *str == '\t' || *str == '\n') ? true : false);
}

char *whitespace_handle(char *str, struct parser_state *state)
{
    #ifdef EXPRESSION_TYPE
            printf("Whitespace:\n");
    #endif
    for(;is_whitespace(str); str++);
    return str;
}


bool is_name(char *in)
{
    char c = *in;
    if(isalnum(c) || c == '_' || c == '.')
    {
        return true;
        #ifdef EXPRESSION_TYPE
            printf("Name:\n");
        #endif
    }
    return false;
}

char *section_modes[] = {"default", "append", "replace", NULL};




char *name_handle(char *str, struct parser_state *state)
{
    if(!is_name(str))
        return str;
    size_t name_len = 0;
    while(is_name(&str[name_len]) && name_len < strlen(str))
        name_len++;
    
    struct pnode *node;

#ifdef DUMP_NAME
    printf("name: ");
    int i;
    for(i = 0; i < name_len; i++)
        printf("%c",str[i]);
    printf("\n");
#endif

    return str+name_len;
}