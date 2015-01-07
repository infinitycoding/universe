/*
     Copyright 2012-2014 Infinitycoding all rights reserved
     This file is part of the universe standart c library.

     The universe standart c library is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     any later version.

     The universe standart c library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with the universe standart c library.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  @author Simon Diepold aka. Tdotu <simon.diepold@infinitycoding.de>
 *  @author Andreas Hahn aka. thedrakor
 *  @copyright GNU Lesser Public License
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

/**
 *  @brief Adds an element before the dummy element (...|last element|new element|dummy|first entry|...).
 *  @param list the list
 *  @param element the element to be added to the list
 *  @return pointer to the list
 */
list_t *list_push_back(list_t *list, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->head->prev);
    return list;
}

/**
 *  @brief Adds an element after of the dummy to a list (...|last element|dummy|new element|first entry|...).
 *  @param list the list
 *  @param element the element to be added to the list
 *  @return pointer to the list
 */
list_t *list_push_front(list_t *list, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->head);
    return list;
}

/**
 *  @brief Internal function which removes a specific node from a list.
 *  @param node the node to be removed
 *  @return pointer to the content of the removed element
 */
void *list_remove_node(struct list_node *node)
{
    void *element = node->element;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    return element;
}

/**
 *  @brief Removes the element before the dummy (complementary to list_pop_back).
 *  @param the list
 *  @pointer to the removed element
 */
void *list_pop_back(list_t *list)
{
    struct list_node *last = list->head->prev;
    void *element = last->element;
    last->prev->next = last->next;
    last->next->prev = last->prev;
    free(last);
    return element;
}

/**
 *  @brief Removes the element after the dummy (complementary to list_pop_front).
 *  @param the list
 *  @pointer to the removed element
 */
void *list_pop_front(list_t *list)
{
    struct list_node *first = list->head->next;
    void *element = first->element;
    first->prev->next = first->next;
    first->next->prev = first->prev;
    free(first);
    return element;
}

void* list_get_by_int(list_t *list, uintptr_t off, int value)
{
    struct list_node *node = list->head->next;
    struct list_node *head = list->head;
    void *element = NULL;

    while (node != head)
    {
        int val1 = *((int*) ((uintptr_t) node->element + off));
        if(val1 == value)
        {
            return node->element;
        }

        node = node->next;
    }

    return NULL;
}

struct list_node* list_get_node_by_int(list_t *list, uintptr_t off, int value)
{
    struct list_node *node = list->head->next;
    struct list_node *head = list->head;

    while (node != head)
    {
        int val1 = *((int*) ((uintptr_t) node->element + off));
        if(val1 == value)
        {
            return node;
        }

        node = node->next;
    }

    return NULL;
}


/**
 *  @brief Counts the number of elements in the given list.
 *  @brief list the list
 *  @return number of elements
 */
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

/**
 *  @brief Checks if the given list is empty.
 *  @param list the list
 *  @return true if the list is empty, false if the list contains elements
 */
bool list_is_empty(list_t *list)
{
    return (list->head == list->head->next);
}

/**
 *  @brief Locks the optional mutex of a list.
 *  @param list the list
 */
void list_lock(list_t *list)
{
    lock(&list->lock);
}

/**
 *  @brief Unlocks the optional mutex of a list.
 *  @param list the list
 */
void list_unlock(list_t *list)
{
    unlock(&list->lock);
}

/**
 *  @breif Creates a new iterator fot a list.
 *  @param list the list
 *  @return the new iterator
 */
iterator_t iterator_create(list_t *list)
{
    iterator_t new_iterator;
    new_iterator.list = list;
    new_iterator.current = list->head->next;
    return new_iterator;
}

/**
 *  @brief Inserts an element after the current element which is selected by the iterator.
 *  @param iterator the iterator
 *  @param element the element to be inserted
 */
void list_insert_after(iterator_t *it, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, it->current);
}

/**
 *  @brief Inserts an element before the current element which is selected by the iterator.
 *  @param iterator the iterator
 *  @param element the element to be inserted
 */
void list_insert_before(iterator_t *it, void *element)
{
    struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, it->current->prev);
}

/**
 *  @brief Get the current element of a list selected by an iterator.
 *  @param the iterator
 *  @return the current element
 */
void *list_get_current(iterator_t *it)
{
    if(it)
        if(it->current)
            return it->current->element;

    return NULL;
}

/**
 *  @brief Switches the current element of an iterator to the next element of it's list. (forward)
 *  @param iterator the iterator
 */
void list_next(iterator_t *it)
{
    it->current = it->current->next;
}

/**
 *  @brief Switches the current element of an iterator to the previous element of it's list.
 *  @param iterator the iterator
 */
void list_previous(iterator_t *it)
{
    it->current = it->current->prev;
}

/**
 *  @brief Checks if the current element is the last before the dummy element or if the list is empty.
 *  @return true if it's the last element, false if it's not.
 */
bool list_is_last(iterator_t *it)
{
    return (it->current == it->list->head);
}

/**
 *  @brief Sets the first element after the dummy as current element of an iterator.
 *  @param iterator the iterator
 */
void list_set_first(iterator_t *it)
{
    it->current = it->list->head->next;
}

/**
 *  @brief Sets the first element before the dummy as current element of an iterator.
 *  @param iterator the iterator
 */
void list_set_last(iterator_t *it)
{
    it->current = it->list->head->prev;
}

/**
 *  @brief Removes the current element from the list and returns it.
 *  @param iterator the iterator
 *  @return the corrent element
 */
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
