#ifndef _list_h_
#define _list_h_

struct ListNode
{
struct ListNode *next;
struct ListNode *prev;
void *element;
};

struct List {
struct ListNode *head;
struct ListNode *current;
};

struct List *ListCreate();
void ListSplice(struct ListNode *start, struct ListNode *end, struct ListNode *target);
struct List *ListPushBack(struct List *list, void *element);
struct List *ListPushFront(struct List *list, void *element);
void *ListPopBack(struct List *list);
void *ListPopFront(struct List *list);
struct List *ListInsertAfter(struct List *list, void *element);
struct List *ListInsertBefore(struct List *list, void *element);
void *ListRemoveNode(struct ListNode *node);
void *ListRemove(struct List *list);
size_t ListSize(struct List *list);
void *ListGetCurrent(struct List *list);
struct List *ListNext(struct List *list);
struct List *ListPrevious(struct List *list);
int ListIsLast(struct List *list);
struct List *ListSetLast(struct List *list);
struct List *ListSetFirst(struct List *list);
bool ListIsEmpty();
#endif
