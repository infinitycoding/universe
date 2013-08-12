#ifndef _list_h_
#define _list_h_

struct ListNode
{
struct ListNode *next;
struct ListNode *prev;
void *element;
};

typedef struct{
struct ListNode *head;
struct ListNode *current;
}List;

List *ListCreate();
void ListSplice(struct ListNode *start, struct ListNode *end, struct ListNode *target);
List *ListPushBack(List *list, void *element);
List *ListPushFront(List *list, void *element);
void *ListPopBack(List *list);
void *ListPopFront(List *list);
List *ListInsertAfter(List *list, void *element);
List *ListInsertBefore(List *list, void *element);
void *ListRemoveNode(struct ListNode *node);
void *ListRemove(List *list);
size_t ListSize(List *list);
void *ListGetCurrent(List *list);
List *ListNext(List *list);
List *ListPrevious(List *list);
int ListIsLast(List *list);
List *ListSetLast(List *list);
List *ListSetFirst(List *list);
bool ListIsEmpty();
#endif
