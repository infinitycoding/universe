#ifndef _cdi_lists_h_
#define _cdi_lists_h_

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
 *  based on the CDI reference implementation by Kevin Wolf
 */

 /**
  * This is actually a wrapper header. If you want to know something about the details
  * take a look at list.c in our std library.
  */

#include <list.h>
#include <stdint.h>


typedef list_t* cdi_list_t;


/**
 * extern "c" declaration for C++ driver programmers...
 */
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Wrapper for list_create.
 * Creates a new list.
 * @return a brand new list! wow!
 */
inline cdi_list_t cdi_list_create(void){
    return list_create();
}

/**
 * Wrapper for list_destroy.
 * Destroys the given list and frees the associated memory.
 */
inline void cdi_list_destroy(cdi_list_t list){
     list_destroy(&list);
}

/**
 * Wrapper for list_push_front.
 * Adds a new entry at position 0 after the dummy in a linked list.
 * @param list the list to insert into
 * @param the entry to be insert
 * @return the given list
 */
inline cdi_list_t cdi_list_push(cdi_list_t list, void* value){
     return list_push_front(list, value);
}

/**
 * Wrapper for list_pop_front.
 * Removes a entry at position 0 after the dummy in a linked list. 
 * @param list The list from which the entry will be removed.
 * @return the entry from position 0
 */
inline void* cdi_list_pop(cdi_list_t list){
     return list_pop_front(list);
}

/**
 * Wrapper for list_is_empty.
 * Checks if the list is empty.
 * @param list The list to be checked
 * @return 1(true) if the list is empty otherwise 0(false)
 */
inline size_t cdi_list_empty(cdi_list_t list){
     return list_is_empty(list);
}

/**
 * Wrapper for list_get.
 * Returns a specific entry from the list.
 * @param list The list from which the entry shuld be taken
 * @param index The position of the entry in the list
 * @return the requested entry or NULL if the element does not exist
 */
inline void* cdi_list_get(cdi_list_t list, size_t index){
     return list_get(list, index);
}

/**
 * Wrapper for list_insert.
 * Inserts a element at a specific position in a list.
 * The elements >=index will be shiftet for one entry in the numeration. 
 * @param list The list to insert into
 * @param index The destination position
 * @return the given list
 */
inline cdi_list_t cdi_list_insert(cdi_list_t list, size_t index, void* value){
     return list_insert(list, index, value);
}

/**
 * Wrapper for list_remove_from_index.
 * Removes a entry form the list at position index.
 * @param list the list from which the entry shuld be taken
 * @param index the postion of the entry in the list
 * @return the removed entry or NULL if the element does not exist
 */
inline void* cdi_list_remove(cdi_list_t list, size_t index){
     return list_remove_from_index(list, index);
}

/**
 * Wrapper for list_length.
 * Returns the number of list entries.
 * @param list The list 
 * @return the size of the list
 */
inline size_t cdi_list_size(cdi_list_t list){
     return (size_t)list_length(list);
}



#ifdef __cplusplus
};
#endif 

#endif


