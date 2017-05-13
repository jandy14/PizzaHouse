#include <stdlib.h>
#include <stdio.h>
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
	for(i = 0; i < PIZZA_MAKER_AMOUNT; ++i)
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

	pizzaHouse->orderCount = Size(pOrderList);
	pizzaHouse->completeCount = 0;

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
void GoNextQue(Order* pOrder, List* pDestinationQue, Scheduling pScheduling)
{
	int index = 0;
	++(pOrder->state);
	if(pScheduling == SJF)
	{
		for(index = 0; index < Size(pDestinationQue); ++index)
		{
			if(pOrder->serviceTime < ((Order*)ObserveItem(pDestinationQue,index))->serviceTime)
				break;
		}
	}
	else
		index = Size(pDestinationQue);
	
	Insert(pDestinationQue,(void*)pOrder,index);	
	return;
}
void FindNextEventTime(PizzaHouse* pPH, int pTime)
{
	if(pPH->nextEventTime == -1)
		pPH->nextEventTime = pTime;
	else if(pPH->nextEventTime > pTime)
		pPH->nextEventTime = pTime;
}
void OrderStart(PizzaHouse* pPH)
{
	if(Size(pPH->orderQue) == 0) //order over
		return;
	while(1)
	{
		if(((Order*)ObserveItem(pPH->orderQue,0))->arrivalTime == pPH->currentTime) //start order!
			GoNextQue((Order*)Pop(pPH->orderQue),pPH->orderQue,pPH->scheduling);
		else if(((Order*)ObserveItem(pPH->orderQue,0))->arrivalTime < pPH->currentTime) // fatal error!
		{
			printf("arrivalTime < currenttime in OrderStart Error\n");
			break;
		}
		else //order over in this time
		{
			FindNextEventTime(pPH, ((Order*)ObserveItem(pPH->orderQue,0))->arrivalTime - pPH->currentTime);
			break;
		}
	}
	return;
}
void ReceiveOrder(PizzaHouse* pPH)
{
	if(pPH->orderReceiver.progressingOrder)	//worker has job.
	{
		pPH->orderReceiver.remainingTime -= pPH->elapsedTime;
		if(pPH->orderReceiver.remainingTime == 0) //worker finish job.
		{
			GoNextQue(pPH->orderReceiver.progressingOrder,pPH->calculateQue,pPH->scheduling);
			RefreshWorker(&(pPH->orderReceiver));
		}
		else if(pPH->orderReceiver.remainingTime < 0) //fatal error
		{
			printf("orderReceiver remainingtime is minus!\n");
			return;
		}
		else
		{
			FindNextEventTime(pPH, pPH->orderReceiver.remainingTime);
		}
	}
	if(Size(pPH->orderQue) != 0 && pPH->orderReceiver.progressingOrder == NULL) //worker remain job.
	{
		pPH->orderReceiver.progressingOrder = (Order*)Pop(pPH->orderQue);
		if(pPH->orderReceiver.progressingOrder->type == VISIT)
		{
			pPH->orderReceiver.remainingTime = VISIT_ORDER_TIME;
			FindNextEventTime(pPH, VISIT_ORDER_TIME);
		}
		else
		{
			pPH->orderReceiver.remainingTime = PHONE_ORDER_TIME;
			FindNextEventTime(pPH, PHONE_ORDER_TIME);
		}
	}
	return;
}
void CalculateOrder(PizzaHouse* pPH)
{
	if(pPH->calculater.progressingOrder)
	{
		pPH->calculater.remainingTime -= pPH->elapsedTime;
		if(pPH->calculater.remainingTime == 0) //worker finish job.
		{
			GoNextQue(pPH->calculater.progressingOrder,pPH->preparationQue,pPH->scheduling);
			RefreshWorker(&(pPH->calculater));
		}
		else if(pPH->calculater.remainingTime < 0) //fatal error
		{
			printf("calculater remainingtime is minus!\n");
			return;
		}
		else
		{
			FindNextEventTime(pPH, pPH->calculater.remainingTime);
		}
	}
	if(Size(pPH->calculateQue) != 0 && pPH->calculater.progressingOrder == NULL)
	{
		pPH->calculater.progressingOrder = (Order*)Pop(pPH->calculateQue);
		pPH->calculater.remainingTime = CALCULATE_TIME;
		FindNextEventTime(pPH, CALCULATE_TIME);
	}
	return;
}