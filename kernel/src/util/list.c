#include <heap.h>
#include <stdint.h>
#include <list.h>

list_t *list_create(void)
{
	list_t *list = (list_t *) malloc(sizeof(list_t));
	struct list_node *dummy = (struct list_node *) malloc(sizeof(struct list_node));
    list->head = dummy;
    list->current = dummy;
    list->lock = false;
    dummy->next = dummy;
    dummy->prev = dummy;
    dummy->element = (void *) 0;
	return list;
}

/* TODO: Check for bugs */
void list_destroy(list_t **list)
{
	struct list_node *node = list[0]->head->next;
	struct list_node *head = list[0]->head;
    while (node != head)
	{

        node = node->next;
		free(node);
    }

	free(*list);
	*list = NULL;
	return;
}

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

void *list_pop_back(list_t *list)
{
	struct list_node *last = list->head->prev;
	return list_remove_node(last);
}

void *list_pop_front(list_t *list)
{
	struct list_node *first = list->head->next;
	return list_remove_node(first);
}

list_t *list_insert_after(list_t *list, void *element)
{
	struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->current);
	return list;
}

list_t *list_insert_before(list_t *list, void *element)
{
	struct list_node *node = (struct list_node *) malloc(sizeof(struct list_node));
    node->element = element;
    node->next = node;
    node->prev = node;
    list_splice(node, node, list->current->prev);
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

void *list_remove(list_t *list)
{
	void *element = list_get_current(list);
	struct list_node *node = list->current;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    list->current = node->next;
	free(node);
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

void *list_get_current(list_t *list)
{
	return list->current->element;
}

list_t *list_next(list_t *list)
{
	list->current = list->current->next;
	return list;
}

list_t *list_previous(list_t *list)
{
	list->current = list->current->prev;
	return list;
}

bool list_is_last(list_t *list)
{
	return (list->current == list->head);
}

list_t *list_set_first(list_t *list)
{
	list->current = list->head->next;
	return list;
}

list_t *list_set_last(list_t *list)
{
	list->current = list->head->prev;
	return list;
}

bool list_is_empty(list_t *list)
{
	return (list->head == list->head->next);
}
