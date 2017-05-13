#ifndef __LIST_H__
#define __LIST_H__

typedef struct _Node Node;
typedef struct _Node List;

struct _Node
{
	void* item;
	Node* next;
	Node* prev;
};

Node* NewList();
void* Pop(Node* head);
int Size(Node* head);
int Insert(Node* head, void* item, int index);
void* ObserveItem(Node* head, int index);
void DeleteList(Node* head);

#endif
//asdgfsag