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
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pfp.h>
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
    state.section_list = list_create();
    state.mode = DEFAULT_MODE;
    state.mode_stack = list_create();
    state.success = true;

    state = parser(state, file, strlen(file));
    printf("\n");
    print_structure(&state);
    return state.success;
}


struct parser_state parser(struct parser_state state, char *file, size_t len)
{
    char *strend = file + len;
    while(state.success && file < strend)
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
    {
        printf("stack %d\n",list_length(state.mode_stack));
        state.success = false;
        // todo: scan stack and print a useful error message
    }
    else
        state.success = true;

    return state;
}



void mode_push(struct parser_state *state)
{
    struct parser_mode *current_mode = malloc(sizeof(struct parser_mode));
    current_mode->mode = state->mode;
    current_mode->node = state->node;
    if(current_mode->node)
        printf("push %s\n",current_mode->node->file);
    else
        printf("push NULL node\n");
    list_push_front(state->mode_stack, current_mode);
}

bool mode_pull(struct parser_state *state)
{
    if(list_is_empty(state->mode_stack))
    {
        printf("tried to pull from a empty stack\n");
        return false;
    }

    
    struct parser_mode *new_mode = (struct parser_mode *)list_pop_front(state->mode_stack);
    printf("file: %s pull %s\n",state->node->file,new_mode->node->file);
    state->mode = new_mode->mode;
    state->node = new_mode->node;
    //free(new_mode); // i dont'get it why it crashes at this point...
    return true;
}