#if _WIN32        // Windows header
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/linkedlist.h"


void createNode(linkedList *L, void *_data, unsigned int size) {
	node *newNode = (node *)malloc(sizeof(node));
	newNode->data = malloc(size);
	memcpy(newNode->data, _data, size);
	newNode->next = NULL;
	if (L->head == NULL && L->tail == NULL)
		L->head = L->tail = newNode;
	else {
		L->tail->next = newNode;
		L->tail = newNode;
	}
}

void deleteLastNode(linkedList *L) {
	node *p = L->head;
	node *temp;
	while (p->next->next != NULL)
	{
		temp = p;
		p = p->next;
		free(temp->data);
		free(temp);
	}
	p->next = p->next->next;
	L->tail = p;
}

void deleteAllNode(linkedList *L)
{
	node *p = L->head;
	node *temp;
	while (p != NULL)
	{
		temp = p;
		p = p->next;
		free(temp);
	}

	free(L);
}

