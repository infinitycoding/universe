#include <heap.h>
#include <stdint.h>
#include <list.h>

struct List *ListCreate()
{
struct List *list = (struct List*)malloc(sizeof(struct List));
struct ListNode *dummy = (struct ListNode*)malloc(sizeof(struct ListNode));
    list->head = dummy;
    list->current = dummy;
    dummy->next = dummy;
    dummy->prev = dummy;
    dummy->element = (void *) 0;
return list;
}

void ListSplice(struct ListNode *start, struct ListNode *end, struct ListNode *target)
{
    start->prev->next = end->next;
    end->next->prev = start->prev;
    start->prev = target;
    end->next = target->next;
    target->next->prev = end;
    target->next = start;

return;
}

struct List *ListPushBack(struct List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->head->prev);
return list;
}

struct List *ListPushFront(struct List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->head);
return list;
}

void *ListPopBack(struct List *list)
{
struct ListNode *last = list->head->prev;
return ListRemoveNode(last);
}

void *ListPopFront(struct List *list)
{
struct ListNode *first = list->head->next;
return ListRemoveNode(first);
}

struct List *ListInsertAfter(struct List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->current);
return list;
}

struct List *ListInsertBefore(struct List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->current->prev);
return list;
}

void *ListRemoveNode(struct ListNode *node)
{
void *element = node->element;
    node->prev->next = node->next;
    node->next->prev = node->prev;
free(node);
return element;
}

void *ListRemove(struct List *list)
{
void *element = ListGetCurrent(list);
struct ListNode *node = list->current;
    node->prev->next = node->next;
    node->next->prev = node->prev;
free(node);
return element;
}

size_t ListSize(struct List *list)
{
struct ListNode *node = list->head->next;
struct ListNode *head = list->head;
size_t size = 0;
    while (node != head) {
        node = node->next;
        size++;
    }
return size;
}

void *ListGetCurrent(struct List *list)
{
return list->current->element;
}

struct List *ListNext(struct List *list)
{
list->current = list->current->next;
return list;
}

struct List *ListPrevious(struct List *list)
{
list->current = list->current->prev;
return list;
}

bool ListIsLast(struct List *list)
{
return (list->current == list->head);
}

struct List *ListSetFirst(struct List *list)
{
list->current = list->head->next;
return list;
}

struct List *ListSetLast(struct List *list)
{
list->current = list->head->prev;
return list;
}

bool ListIsEmpty(struct List *list)
{
return (list->head == list->head->next);
}


