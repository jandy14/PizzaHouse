// typedef struct _Order Order;
// typedef struct _Pizza Pizza;
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
void DeleteList(Node* head);