#include <stdio.h>
#include "List.h"

#define PIZZAMAKERAMOUNT 5

typedef enum
{
	PHONE,	//phoning order
	VISIT,	//visiting order
	ORDERTYPECOUNT	//it just for count
}OrderType;

typedef enum
{
	BeforeOrder = 0,
	WaitingToOrdered,
	Ordering,
	WaitingToCalculated,
	Calculating,
	WaitingToMaked,
	WaitingPizza,
	WaitingToDelivered,
	Delivering,
	Complete
}OrderState;

typedef enum
{
	CHEESE = 0,	//Pizza with lots of cheese
	PEPPERONI,	//Pizza with Salty of pepperoni
	POTATO,		//Pizza with Delicious potato
	SHRIMP,		//Pizza with fresh shrimp
	PIZZATYPECOUNT	//it just for count
}PizzaType;

typedef enum
{
	CHEESE_TIME = 80,	//cheese pizza need 80sec to make
	PEPPERONI_TIME = 90,	//pepperoni pizza need 90sec to make
	POTATO_TIME = 100,	//potato pizza need 100sec to make
	SHRIMP_TIME = 110,	//shrimp pizza need 110sec to make
}MakingTime;

typedef struct _Order Order;
typedef struct _Pizza Pizza;
typedef struct _Worker Worker;
typedef struct _PizzaMaker PizzaMaker;
typedef struct _PizzaHouse PizzaHouse;

struct _Pizza
{
	Order* order;		//order that contain this pizza
	int remainingTime;	//the second it takes for the complete pizza
	PizzaType type;		//pizza's type
};

struct _Order
{
	int num;			//order number
	int arrivalTime;	//hours convert to seconds
	OrderType type;		//is phone or visit
	Pizza* pizzaList;	//ordered pizza array
	int serviceTime;	//It is calculated by pizzaList
	int timer;			//progress time
};

struct _Worker
{
	Order* progressingOrder;	//order in process
	int remainingTime;			//remaining time to the end this work
};

struct _PizzaMaker
{
	Pizza* progressingPizza;	//pizza in process
	Pizza** workQue;			//pizza list to make
	int remainingTime;			//remaining time to the end this work
};

struct _PizzaHouse
{
	int currentTime;
	int nextEventTime;
	int elapsedTime;
	Order* OrderList;		//order array

	Worker orderReceiver;
	Worker calculater;
	Worker deliveryStaff;
	PizzaMaker pizzaMaker[PIZZAMAKERAMOUNT];

	List* orderQue;
	List* calculateQue;
	List* preparationQue;
	List* makingPizzaQue;
	List* waitingPizzaQue;
	List* deliveryQue;
};
int main()
{
// 	Pizza a;
// 	Order b;
// 	printf("%d\n",b.num);
// 	a.remainingTime = 100;

// 	a.remainingTime -= 10;
// 	a.order = &b;
// 	a.order->num = 123;
// 	printf("%d\n",a.remainingTime);
// 	printf("%d\n",b.num);
// 	return 0;

	List* testList = NewList();
	int size;

	Insert(testList, (void*)"hi",0);
	Insert(testList, (void*)"hello",0);
	Insert(testList, (void*)"bye",0);
	Insert(testList, (void*)"what", 1);

	printf("%d\n",Size(testList));

	// printf("%s\n",(char*)Pop(testList));
	// printf("%s\n",(char*)Pop(testList));
	// printf("%s\n",(char*)Pop(testList));
	// printf("%s\n",(char*)Pop(testList));
	//printf("%s\n",(char*)Pop(testList));

	DeleteList(testList);

	printf("%d\n",Size(testList));
		
	return 0;
}