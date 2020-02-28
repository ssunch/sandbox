#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct __list {
	struct __node *cur;
	struct __node *head;
	struct __node *tail;
} linkedList, *plinkedList;

typedef struct __node {
	void *data;
	struct __node *next;
} node;

void createNode(linkedList *L, void *_data, unsigned int size);
void deleteLastNode(linkedList *L);
void deleteAllNode(linkedList *L);

#endif