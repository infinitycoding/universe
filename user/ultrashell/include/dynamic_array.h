/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the Ultrashell.
 
     The Ultrashell is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.
 
     The Ultrashell is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.
 
     You should have received a copy of the GNU Lesser General Public License
     along with the Ultrashell.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @author Peter Hösch aka. BlitzBasic <peter.hoesch@infinitycoding.de>
 **/



#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

//#define DYN_ARRAY_DEBUG

typedef struct dynamic_array_block
{
     char *value;
     struct dynamic_array_block *next;
}dynamic_array_block;

typedef struct dynamic_array
{
     int blocksize;
     struct dynamic_array_block *first;
}dynamic_array;

dynamic_array *create_array(int blcksize);
dynamic_array *delete_array(dynamic_array *array);
char get(dynamic_array *array, int nr);
char set(dynamic_array *array, int nr, char val);
char *as_string(dynamic_array *array);

#endif

