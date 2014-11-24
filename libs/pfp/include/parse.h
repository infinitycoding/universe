#ifndef _lbps_parser_h_
#define _lbps_parser_h_

/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the mercury c-library.
 
     The mercury c-library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The mercury c-library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the mercury c-library.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @copyright GNU Lesser Public License
 */

#include <stdint.h>
#include <error.h>
#include <graph.h>




typedef struct
{
	size_t index;
}parser;


parser *createParser();
char *getError(parser *p);
void destroyParser(parser *p);

graph *parsf(parser *p, const char *str,const char *fotmat, ...);
char *exportf(parser *p, graph *g, const char *fotmat, ...);

#endif


