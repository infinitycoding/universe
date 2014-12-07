#ifndef	_XML_H_
#define	_XML_H_

#include <list.h>

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
 */ 

typedef struct
{
	char *name;
	char *value;
}xml_param;

struct xml_node
{
	char *name;
	char *text;
	list_t *params;
	list_t *subnodes;
	struct xml_node *parent;
};

typedef struct xml_node xml_node;

typedef enum
{
	STATUS_TEXT = 0,
	STATUS_TAG_NAME = 1,
	STATUS_TAG_SPACE =  2,
	STATUS_PARAM_NAME = 3,
	STATUS_PARAM_VALUE = 4,
	STATUS_TAG_END = 5
}parser_status_t;



#endif