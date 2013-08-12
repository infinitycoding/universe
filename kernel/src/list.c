#include <heap.h>
#include <stdint.h>
#include <list.h>

List *ListCreate()
{
List *list = (List*)malloc(sizeof(List));
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

List *ListPushBack(List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->head->prev);
return list;
}

List *ListPushFront(List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->head);
return list;
}

void *ListPopBack(List *list)
{
struct ListNode *last = list->head->prev;
return ListRemoveNode(last);
}

void *ListPopFront(List *list)
{
struct ListNode *first = list->head->next;
return ListRemoveNode(first);
}

List *ListInsertAfter(List *list, void *element)
{
struct ListNode *node = (struct ListNode*) malloc(sizeof(struct ListNode));
    node->element = element;
    node->next = node;
    node->prev = node;
    ListSplice(node, node, list->current);
return list;
}

List *ListInsertBefore(List *list, void *element)
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

void *ListRemove(List *list)
{
void *element = ListGetCurrent(list);
struct ListNode *node = list->current;
    node->prev->next = node->next;
    node->next->prev = node->prev;
free(node);
return element;
}

size_t ListSize(List *list)
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

void *ListGetCurrent(List *list)
{
return list->current->element;
}

List *ListNext(List *list)
{
list->current = list->current->next;
return list;
}

List *ListPrevious(List *list)
{
list->current = list->current->prev;
return list;
}

bool ListIsLast(List *list)
{
return (list->current == list->head);
}

List *ListSetFirst(List *list)
{
list->current = list->head->next;
return list;
}

List *ListSetLast(List *list)
{
list->current = list->head->prev;
return list;
}

bool ListIsEmpty(List *list)
{
return (list->head == list->head->next);
}


