#include <stdlib.h>
#include <stdio.h>

#include "List.h"

Node* NewList()
{
	Node* head = (Node*)malloc(sizeof(Node));
	head->item = NULL;
	head->prev = NULL;
	head->next = NULL;

	return head;
}

void* Pop(Node* pHead)
{
	void* item;
	Node* popNode;

	if(pHead->next == NULL)
		return NULL;
	
	item = pHead->next->item;
	popNode = pHead->next;

	if(popNode->next == NULL)
		pHead->next = NULL;
	else
	{
		popNode->next->prev = pHead;
		pHead->next = pHead->next->next;
	}
	free(popNode);
	return item;
}

int Size(Node* pHead)
{
	int size = 0;
	Node* iter;
	for(iter = pHead; iter->next; iter = iter->next)
		++size;
	return size;
}

int Insert(Node* pHead, void* pItem, int pIndex)
{
	int size = Size(pHead);
	Node* selectNode;
	int i = 0;
	Node* newNode;

	if(pIndex < 0 || pIndex > size)
		return -1;

	for(i = 0, selectNode = pHead; i < pIndex; ++i)
		selectNode = selectNode->next;

	newNode = (Node*)malloc(sizeof(Node));
	newNode->item = pItem;

	newNode->prev = selectNode;
	if(selectNode->next == NULL)
		newNode->next = NULL;
	else
		newNode->next = selectNode->next;
	selectNode->next = newNode;
	if(newNode->next != NULL)
		newNode->next->prev = newNode;

	return 0;
}

void* ObserveItem(Node* pHead, int pIndex)
{
	//printf("HI\n");
	Node* iter = pHead;
	int i;
	if(pIndex < 0 || pIndex >= Size(pHead))
		return NULL;
	//printf("HI\n");
	for(i = 0; i <= pIndex; ++i)
		iter = iter->next;
	//printf("HI\n");
	return iter->item;
}

void DeleteList(Node* pHead)
{
	Node* iter;

	if(pHead->next == NULL)
	{
		free(pHead);
		return;
	}

	for(iter = pHead->next; iter->next; iter = iter->next)
		free(iter->prev);

	free(iter->prev);
	free(iter);

	return;
}