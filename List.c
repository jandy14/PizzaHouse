// -----------------------------------------------------------------------
// NAME : Jung Sung-wook	User ID : 20123307
// DUE DATE : 05/17/2017
// PROJECT #1
// FILE NAME : List.c
// PROGRAM PURPOSE :
//	List.h에 선언된 함수를 정의 합니다.
// -----------------------------------------------------------------------
#include <stdlib.h>
#include "List.h"

// -----------------------------------------------------------------------
// FUNCTION NewList :
//	Allocate List head node and return
// -----------------------------------------------------------------------
Node* NewList()
{
	Node* head = (Node*)malloc(sizeof(Node));
	head->item = NULL;
	head->prev = NULL;
	head->next = NULL;

	return head;
}

// -----------------------------------------------------------------------
// FUNCTION Pop :
//	리스트의 제일 처음 노드에 저장된 값을 반환하고 리스트에서 제거
// PARAMETER USAGE :
//	pHead : 리스트 헤더
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION Size :
//	리스트가 가지고 있는 노드의 갯수를 확인하고 반환
// PARAMETER USAGE :
//	pHead : 리스트 헤더
// -----------------------------------------------------------------------
int Size(Node* pHead)
{
	int size = 0;
	Node* iter;
	for(iter = pHead; iter->next; iter = iter->next)
		++size;
	return size;
}

// -----------------------------------------------------------------------
// FUNCTION Insert :
//	리스트의 정해진 인덱스에 값을 저장하는 함수
// PARAMETER USAGE :
//	pHead : 리스트 헤더
//	pItem : 리스트에 저장될 값
//	pIndex : 리스트의 몇번쩨에 넣을 것인가
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION ObserveItem :
//	리스트의 정해진 인덱스에 있는 값을 반환 (리스트에서 없애지는 않음)
// PARAMETER USAGE :
//	pHead : 리스트 헤더
//	pIndex : 리스트의 몇번쩨를 볼것인가
// -----------------------------------------------------------------------
void* ObserveItem(Node* pHead, int pIndex)
{

	Node* iter = pHead;
	int i;
	if(pIndex < 0 || pIndex >= Size(pHead))
		return NULL;

	for(i = 0; i <= pIndex; ++i)
		iter = iter->next;

	return iter->item;
}

// -----------------------------------------------------------------------
// FUNCTION DeleteList :
//	리스트에 연결되어있는 모든 노드들의 메모리를 풀어주는 함수
// PARAMETER USAGE :
//	pHead : 리스트 헤더
// -----------------------------------------------------------------------
void DeleteList(Node* pHead)
{
	Node* iter;
	if(pHead == NULL)
		return;
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