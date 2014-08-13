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
/*char *section_handle(char *str, struct parser_state *state);
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
char *end_section_type_handle(char *str, struct parser_state *state);*/


char *identifiers[] = {"#", "section", "service", "->", "|", "[", "]", "~{", "}", ";", ":", "," ,"(", ")", NULL};
char *(*identifier_handles[])(char *str, struct parser_state *state) =
{
    comment_handle, NULL /*section_handle*/, NULL /*service_handle*/, NULL /*kernel_root_handle*/, NULL /*pipe_handle*/,
    NULL /*open_square_brace_handle*/, NULL /*close_square_brace_handle*/, NULL /*fallback_handle*/, NULL /*end_fallback_handle*/, NULL /*semicolon_handle*/,
    NULL /*colon_handle*/, NULL /*comma_handle*/, NULL /*section_type_handle*/, NULL /*end_section_type_handle*/, NULL
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

    printf("Identifier %s\n", identifiers[num]);
    if(identifier_handles[num] == NULL)
    {
        
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
