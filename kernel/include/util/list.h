#ifndef _LIST_H_
#define _LIST_H_

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
 *  @author Andreas Hahn aka. thedrakor
 */


struct list_node
{
	struct list_node *next;
	struct list_node *prev;
	void *element;
};

typedef struct
{
	struct list_node *head;
	struct list_node *current;
	bool lock;
} list_t;

list_t*	list_create(void);
void 	list_destroy(list_t **list);
void 	list_splice(struct list_node *start, struct list_node *end, struct list_node *target);
list_t*	list_push_back(list_t *list, void *element);
list_t*	list_push_front(list_t *list, void *element);
void*	list_pop_back(list_t *list);
void*	list_pop_front(list_t *list);
list_t*	list_insert_after(list_t *list, void *element);
list_t*	list_insert_before(list_t *list, void *element);
void*	list_remove_node(struct list_node *node);
void*	list_remove(list_t *list);
int		list_length(list_t *list);
void*	list_get_current(list_t *list);
list_t*	list_next(list_t *list);
list_t*	list_previous(list_t *list);
int 	list_is_last(list_t *list);
list_t*	list_set_last(list_t *list);
list_t*	list_set_first(list_t *list);
bool	list_is_empty();

#endif
