#include <stdlib.h>
#include "PizzaHouse.h"
#include "List.h"

PizzaHouse* PizzaHouseOpen(List* pOrderList, int pQuantum, Scheduling pScheduling)
{
	int i;
	PizzaHouse* pizzaHouse = (PizzaHouse*)malloc(sizeof(PizzaHouse));

	pizzaHouse->quantum = pQuantum;
	pizzaHouse->scheduling = pScheduling;
	pizzaHouse->orderList = pOrderList;

	pizzaHouse->currentTime = ((Order*)ObserveItem(pOrderList,0))->arrivalTime;
	pizzaHouse->nextEventTime = -1;
	pizzaHouse->elapsedTime = 0;

	RefreshWorker(&(pizzaHouse->orderReceiver));
	RefreshWorker(&(pizzaHouse->calculater));
	RefreshWorker(&(pizzaHouse->deliveryStaff));
	for(i = 0; i < PIZZAMAKERAMOUNT; ++i)
	{
		RefreshPizzaMaker(&(pizzaHouse->pizzaMaker[i]),0);
		if(pScheduling == RR)
			pizzaHouse->pizzaMaker[i].workQue = NewList();
		else
			pizzaHouse->pizzaMaker[i].workQue = NULL;
	}

	pizzaHouse->orderQue = NewList();
	pizzaHouse->calculateQue = NewList();
	pizzaHouse->preparationQue = NewList();
	pizzaHouse->makingPizzaQue = NewList();
	pizzaHouse->waitingPizzaQue = NewList();
	pizzaHouse->deliveryQue = NewList();

	return pizzaHouse;
}
void RefreshWorker(Worker* pTarget)
{
	pTarget->progressingOrder = NULL;
	pTarget->remainingTime = 0;
}
void RefreshPizzaMaker(PizzaMaker* pTarget, int pIsAllClear)
{
	pTarget->progressingPizza = NULL;
	pTarget->remainingTime = 0;
	if(pIsAllClear)
		DeleteList(pTarget->workQue);
}