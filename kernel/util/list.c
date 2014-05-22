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
 *  @copyright GNU Public License
 */

#include <mm/heap.h>
#include <stdint.h>
#include <list.h>

/**
 *  @brief Creates a linked list.
 *  @return new list
 */
list_t *list_create(void)
{
	list_t *list = (list_t *) malloc(sizeof(list_t));
	struct list_node *dummy = (struct list_node *) malloc(sizeof(struct list_node));
    list->head = dummy;
    dummy->next = dummy;
    dummy->prev = dummy;
    dummy->element = (void *) 0;
    unlock(&list->lock);
	return list;
}


/**
 *  @brief Destroys a list.
 *  @param list the list to be destroied
 */
void list_destroy(list_t *list)
{
	struct list_node *node = list->head->next;
	struct list_node *head = list->head;
    while (node != head)
	{
        node = node->next;
		free(node);
    }

	free(list);
	return;
}


/**
 *  @brief Moves all elements between start and end after target.
 *  @param start Start of the element chain
 *  @param end end of the element chain
 *  @param target Target place
 */
void list_splice(struct list_node *start, struct list_node *end, struct list_node *target)
{
    start->prev->next = end->next;
    end->next->prev = start->prev;
    start->prev = target;
    end->next = target->next;
    target->next->prev = end;
    target->next = start;
	return;
}


list_t *list_push_back(list_t *list, void *element)
{
	struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->head->prev);
	return list;
}

list_t *list_push_front(list_t *list, void *element)
{
	struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->head);
	return list;
}

void *list_remove_node(struct list_node *node)
{
    void *element = node->element;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    return element;
}

void *list_pop_back(list_t *list)
{
	struct list_node *last = list->head->prev;
    void *element = last->element;
    last->prev->next = last->next;
    last->next->prev = last->prev;
    free(last);
    return element;
}

void *list_pop_front(list_t *list)
{
	struct list_node *first = list->head->next;
    void *element = first->element;
    first->prev->next = first->next;
    first->next->prev = first->prev;
    free(first);
    return element;
}


int list_length(list_t *list)
{
	struct list_node *node = list->head->next;
	struct list_node *head = list->head;
	size_t size = 0;
    while (node != head)
	{
        node = node->next;
        size++;
    }
	return size;
}

bool list_is_empty(list_t *list)
{
    return (list->head == list->head->next);
}

void list_lock(list_t *list)
{
    lock(&list->lock);
}

void list_unlock(list_t *list)
{
    unlock(&list->lock);
}


// using interators


iterator_t iterator_create(list_t *list)
{
    iterator_t new_iterator;
    new_iterator.list = list;
    new_iterator.current = list->head->next;
    return new_iterator;
}



void list_insert_after(iterator_t *it, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, it->current);
}


void list_insert_before(iterator_t *it, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, it->current->prev);
}


void *list_get_current(iterator_t *it)
{
    if(it)
        if(it->current)
            return it->current->element;

    return NULL;
}

void list_next(iterator_t *it)
{
    it->current = it->current->next;
}

void list_previous(iterator_t *it)
{
    it->current = it->current->prev;
}

bool list_is_last(iterator_t *it)
{
    return (it->current == it->list->head);
}

void list_set_first(iterator_t *it)
{
    it->current = it->list->head->next;
}

void list_set_last(iterator_t *it)
{
    it->current = it->list->head->prev;
}

void *list_remove(iterator_t *it)
{
    void *element = list_get_current(it);
    struct list_node *node = it->current;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    it->current = node->next;
    free(node);
    return element;
}
