#ifndef _list_h_
#define _list_h_

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
bool	list_is_empty(list_t *list);
void*   list_get(list_t *list, size_t index);

#endif
