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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pfp.h>
#include <tokens.h>
#include <stdio.h>

extern char *call_identifier_handle(int num, char *str, struct parser_state *state);
char *call_expression_handle(int num, char *str, struct parser_state *state);
struct parser_state parser(struct parser_state state, char *file, size_t len);


/**
 * @brief validates a pipeline file
 * @param file the pipeline file as a ascii string
 * @return true if the pipeline file is valid
 * @return false if the pipeline file is invalid
 */
int validate_pf(char *file)
{
    struct parser_state state;
    state.section = NULL;
    state.node = NULL;
    state.success = true;
    state.mode = DEFAULT_MODE;
    state.mode_stack = list_create();
    parser(state, file, strlen(file));
    return state.success;
}


struct parser_state parser(struct parser_state state, char *file, size_t len)
{
    char *strend = file + len;
    while(file < strend)
    {
        int id = is_identifier(file);

        if(id >= 0)
        {
            file = call_identifier_handle(id, file, &state);
            continue;
        }
        int expr = is_expression(file);
        if(expr >= 0)
        {
            file = call_expression_handle(expr, file, &state);
            continue;
        }
        
        printf("can not identify token\n");
        state.success = false;
        return state;
    }

    if(list_length(state.mode_stack))
        state.success = false;
    else
        state.success = true;
    return state;
}