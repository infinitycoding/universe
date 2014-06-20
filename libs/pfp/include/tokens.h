#ifndef	_tokens_h_
#define	_tokens_h_

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
 **/
#include <sys/types.h>
#include <pfp.h>

int is_identifier(char *str);
bool is_whitespace(char *character);
char *skip_whitespaces(char *str);
bool is_comment(char *in);
size_t comment_lengh(char *in);
bool is_name(char *in);
size_t name_lengh(char *in);
int is_expression(char *str);


char *comment_handle(char *str, struct parser_state *state);
char *section_handle(char *str, struct parser_state *state);
char *kernel_root_handle(char *str, struct parser_state *state);
char *pipe_handle(char *str, struct parser_state *state);
char *semicolon_handle(char *str, struct parser_state *state);
char *colon_handle(char *str, struct parser_state *state);
char *service_handle(char *str, struct parser_state *state);
char *open_square_brace_handle(char *str, struct parser_state *state);
char *name_handle(char *str, struct parser_state *state);
char *whitespace_handle(char *str, struct parser_state *state);



#endif