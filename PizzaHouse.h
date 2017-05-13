#ifndef __PIZZAHOUSE_H__
#define __PIZZAHOUSE_H__

#include "List.h"

#define PIZZAMAKERAMOUNT 5

typedef enum
{
	FCFS = 0,
	SJF,
	RR
}Scheduling;

typedef enum
{
	PHONE = 0,	//phoning order
	VISIT,		//visiting order
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
	SHRIMP_TIME = 110	//shrimp pizza need 110sec to make
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
	List* pizzaList;	//ordered pizza array
	int serviceTime;	//It is calculated by pizzaList
	int timer;			//progress time
	OrderState state;	//order's current state
};

struct _Worker
{
	Order* progressingOrder;//order in process
	int remainingTime;		//remaining time to the end this work
};

struct _PizzaMaker
{
	Pizza* progressingPizza;//pizza in process
	List* workQue;			//pizza list to make
	int remainingTime;		//remaining time to the end this work
};

struct _PizzaHouse
{
	int quantum;
	Scheduling scheduling;

	List* orderList;	//all order list
	int currentTime;
	int nextEventTime;
	int elapsedTime;	//time difference between previous and current event

	Worker orderReceiver;
	Worker calculater;
	Worker deliveryStaff;
	PizzaMaker pizzaMaker[PIZZAMAKERAMOUNT];

	List* orderQue;
	List* calculateQue;
	List* preparationQue;
	List* makingPizzaQue;	//pizza's list
	List* waitingPizzaQue;
	List* deliveryQue;
};

PizzaHouse* PizzaHouseOpen(List* orderList, int quantum, Scheduling scheduling);
void RefreshWorker(Worker* target);
void RefreshPizzaMaker(PizzaMaker* target, int isAllClear);
#endif
