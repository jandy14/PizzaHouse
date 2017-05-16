#include <stdlib.h>
#include <stdio.h>
#include "PizzaHouse.h"
#include "List.h"

PizzaHouse* PizzaHouseOpen(List* pOrderList, int pQuantum, Scheduling pScheduling, FILE* pDestination)
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

	pizzaHouse->destination = pDestination;

	PrintOrder(NULL, 0, NULL, 0);

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

	//printf("%d vs %d\n", pPH->nextEventTime, pTime);

	return;
}
void OrderStart(PizzaHouse* pPH)
{
	if(Size(pPH->orderList) == 0) //order over
		return;
	while(1)
	{
		if(((Order*)ObserveItem(pPH->orderList,0))->arrivalTime == pPH->currentTime) //start order!
		{
			//printf("orderlist size : %d\n",Size(pPH->orderList));
			//printf("pop!\n");
			Order* order = (Order*)Pop(pPH->orderList);
			//PrintOrder(order,pPH->currentTime,1);
			GoNextQue(order,pPH->orderQue,pPH->scheduling);
			//printf("orderlist size : %d\n",Size(pPH->orderList));
			if(Size(pPH->orderList) == 0)
				break;
		}
		else if(((Order*)ObserveItem(pPH->orderList,0))->arrivalTime < pPH->currentTime) // fatal error!
		{
			printf("arrivalTime < currenttime in OrderStart Error\n");
			printf("%d vs %d\n",((Order*)ObserveItem(pPH->orderList,0))->arrivalTime, pPH->currentTime);
			break;
		}
		else //order over in this time
		{
			//printf("hi\n");
			FindNextEventTime(pPH, ((Order*)ObserveItem(pPH->orderList,0))->arrivalTime - pPH->currentTime);
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
		++(pPH->orderReceiver.progressingOrder->state);
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
		++(pPH->calculater.progressingOrder->state);
		pPH->calculater.remainingTime = CALCULATE_TIME;
		FindNextEventTime(pPH, CALCULATE_TIME);
	}
	return;
}
void MakePizza(PizzaHouse* pPH)
{
	if(pPH->scheduling == RR)
		DistributeWork(pPH);
	//printf("Go\n");
	int i;
	for(i = 0; i < PIZZA_MAKER_AMOUNT; i++)
	{
		if(pPH->pizzaMaker[i].progressingPizza)
		{
			pPH->pizzaMaker[i].progressingPizza->remainingTime -= pPH->elapsedTime;
			pPH->pizzaMaker[i].remainingTime -= pPH->elapsedTime;

			if(pPH->pizzaMaker[i].remainingTime == 0)
			{
				if(pPH->pizzaMaker[i].progressingPizza->remainingTime == 0)
				{
					RefreshPizzaMaker(&(pPH->pizzaMaker[i]),0);
				}
				else
				{
					Insert(pPH->pizzaMaker[i].workQue,(void*)pPH->pizzaMaker[i].progressingPizza,Size(pPH->pizzaMaker[i].workQue));
					RefreshPizzaMaker(&(pPH->pizzaMaker[i]),0);
				}
			}
			else
				FindNextEventTime(pPH, pPH->pizzaMaker[i].remainingTime);
		}
		if(pPH->scheduling == RR)
		{
			if(Size(pPH->pizzaMaker[i].workQue) == 0 || pPH->pizzaMaker[i].progressingPizza != NULL)
				continue;
			pPH->pizzaMaker[i].progressingPizza = (Pizza*)Pop(pPH->pizzaMaker[i].workQue);
			if(pPH->quantum > pPH->pizzaMaker[i].progressingPizza->remainingTime)
				pPH->pizzaMaker[i].remainingTime = pPH->pizzaMaker[i].progressingPizza->remainingTime;
			else
				pPH->pizzaMaker[i].remainingTime = pPH->quantum;
			FindNextEventTime(pPH, pPH->pizzaMaker[i].remainingTime);
		}
		else
		{
			if(pPH->pizzaMaker[i].progressingPizza == NULL)
			{
				if(Size(pPH->makingPizzaQue) == 0)
				{
					if(Size(pPH->preparationQue) == 0)
						continue;
					else
						ReadyPizza((Order*)Pop(pPH->preparationQue), pPH->makingPizzaQue, pPH->waitingPizzaQue);
				}
				pPH->pizzaMaker[i].progressingPizza = (Pizza*)Pop(pPH->makingPizzaQue);
				pPH->pizzaMaker[i].remainingTime = pPH->pizzaMaker[i].progressingPizza->remainingTime;
				FindNextEventTime(pPH, pPH->pizzaMaker[i].remainingTime);
			}
		}
	}
	return;
}
void DistributeWork(PizzaHouse* pPH)
{
	static int index = 0;
	while(1)
	{
		Order* item;
		if((item = (Order*)Pop(pPH->preparationQue)) == NULL)
			break;
		ReadyPizza(item, pPH->makingPizzaQue, pPH->waitingPizzaQue);
	}

	while(1)
	{
		Pizza* item = (Pizza*)Pop(pPH->makingPizzaQue);
		if(item == NULL)
			break;
		Insert(pPH->pizzaMaker[index].workQue, (void*)item, Size(pPH->pizzaMaker[index].workQue));
		++index;
		if(index == PIZZA_MAKER_AMOUNT)
			index = 0;
	}
	return;
}
void ReadyPizza(Order* pOrder, List* pPizzaList ,List* pNextQue)
{
	int i;
	for(i = 0; i < Size(pOrder->pizzaList); ++i)
		Insert(pPizzaList, ObserveItem(pOrder->pizzaList, i), Size(pPizzaList));
	GoNextQue(pOrder, pNextQue, FCFS);
	return;
}
void PrintOrder(Order* pOrder, int pCurrentTime, FILE* pOutput, int pIsTest)
{
	static int num = 1;

	if(pOrder == NULL)
	{
		num = 1;
		return;
	}

	if(pIsTest)
	{
		fprintf(pOutput, "\t%-2d ", pOrder->num);
	}
	else
	{
		fprintf(pOutput, "%-2d ", num);
		++num;
	}

	fprintf(pOutput, "%02d:%02d ",pOrder->arrivalTime/(60*60),(pOrder->arrivalTime/60)%60);
	if(pOrder->type == VISIT)
		fprintf(pOutput, "visiting ");
	else
		fprintf(pOutput, "phone    ");
	fprintf(pOutput, "%-4d ",pOrder->serviceTime);
	fprintf(pOutput, "%-4d ",pCurrentTime - pOrder->arrivalTime);

	if(pIsTest)
	{
		switch(pOrder->state)
		{
		case BeforeOrder:
			fprintf(pOutput, "BeforeOrder");
			break;
		case WaitingToOrdered:
			fprintf(pOutput, "WaitingToOrdered");
			break;
		case Ordering:
			fprintf(pOutput, "Ordering");
			break;
		case WaitingToCalculated:
			fprintf(pOutput, "WaitingToCalculated");
			break;
		case Calculating:
			fprintf(pOutput, "Calculating");
			break;
		case WaitingToMaked:
			fprintf(pOutput, "WaitingToMaked");
			break;
		case WaitingPizza:
			fprintf(pOutput, "WaitingPizza");
			break;
		case WaitingToDelivered:
			fprintf(pOutput, "WaitingToDelivered");
			break;
		case Delivering:
			fprintf(pOutput, "Delivering");
			break;
		case Complete:
			fprintf(pOutput, "Complete");
			break;
		default:
			fprintf(pOutput, "STATE ERROR");
		}
	}
	fprintf(pOutput, "\n");
	return;
}
void CheckCompletedOrder(PizzaHouse* pPH)
{
	int listSize = Size(pPH->waitingPizzaQue);
	int i,j;

	for(i = 0; i < listSize; ++i)
	{
		int isAllComplete = 1;
		//Order* order = (Order*)ObserveItem(pPH->waitingPizzaQue,i);
		Order* order = (Order*)Pop(pPH->waitingPizzaQue);
		int pizzaListSize = Size(order->pizzaList);
		for(j = 0; j < pizzaListSize; ++j)
		{
			Pizza* pizza = (Pizza*)ObserveItem(order->pizzaList,j);
			if(pizza->remainingTime)
			{
				isAllComplete = 0;
				break;
			}
		}
		if(isAllComplete)
		{
			if(order->type == VISIT)
			{
				order->state = Complete;
				PrintOrder(order,pPH->currentTime,pPH->destination,0);
				FreeOrderMemory(order);
				++(pPH->completeCount);
			}
			else
				GoNextQue(order, pPH->deliveryQue, FCFS);
		}
		else
			Insert(pPH->waitingPizzaQue,(void*)order,Size(pPH->waitingPizzaQue));
	}
}
void FreeOrderMemory(Order* pOrder)
{
	DeleteList(pOrder->pizzaList);
	free(pOrder);
}
void Delivery(PizzaHouse* pPH)
{
	if(pPH->deliveryStaff.progressingOrder)
	{
		pPH->deliveryStaff.remainingTime -= pPH->elapsedTime;
		if(pPH->deliveryStaff.remainingTime == 0)
		{
			++(pPH->deliveryStaff.progressingOrder->state);
			PrintOrder(pPH->deliveryStaff.progressingOrder,pPH->currentTime,pPH->destination,0);
			FreeOrderMemory(pPH->deliveryStaff.progressingOrder);
			++(pPH->completeCount);
			RefreshWorker(&(pPH->deliveryStaff));
		}
		else if(pPH->deliveryStaff.remainingTime < 0) //fatal error
		{
			printf("deliveryStaff remainingtime is minus!\n");
			return;
		}
		else
		{
			FindNextEventTime(pPH, pPH->deliveryStaff.remainingTime);
		}
	}
	if(Size(pPH->deliveryQue) != 0 && pPH->deliveryStaff.progressingOrder == NULL)
	{
		pPH->deliveryStaff.progressingOrder = (Order*)Pop(pPH->deliveryQue);
		++(pPH->deliveryStaff.progressingOrder->state);
		pPH->deliveryStaff.remainingTime = DELIVERY_TIME;
		FindNextEventTime(pPH, DELIVERY_TIME);
	}
}
BOOL PizzaHouseRun(PizzaHouse* pPH)
{
	if(pPH->nextEventTime == -1)
		pPH->elapsedTime = 0;
	else
		pPH->elapsedTime = pPH->nextEventTime;
	pPH->currentTime += pPH->elapsedTime;
	pPH->nextEventTime = -1;

	OrderStart(pPH);
	ReceiveOrder(pPH);
	CalculateOrder(pPH);
	MakePizza(pPH);
	CheckCompletedOrder(pPH);
	Delivery(pPH);

	return !(IsAllOrderClear(pPH));
}
BOOL IsAllOrderClear(PizzaHouse* pPH)
{
	return pPH->orderCount == pPH->completeCount? TRUE:FALSE;
}
void PizzaHouseClose(PizzaHouse* pPH)
{
	int i;
	DeleteList(pPH->orderList);

	for(i = 0; i < PIZZA_MAKER_AMOUNT; ++i)
		DeleteList(pPH->pizzaMaker[i].workQue);

	DeleteList(pPH->orderQue);
	DeleteList(pPH->calculateQue);
	DeleteList(pPH->preparationQue);
	DeleteList(pPH->makingPizzaQue);
	DeleteList(pPH->waitingPizzaQue);
	DeleteList(pPH->deliveryQue);

	free(pPH);

	return;
}
void PizzaHouseNowState(PizzaHouse* pPH, FILE* pDebugFile)
{
	int i;
	int current = pPH->currentTime;
	fprintf(pDebugFile, "CurrentTime : %02d:%02d:%02d\n",current/(60*60), (current/60)%60, current%60);
	if(Size(pPH->orderQue)!=0)
	{
		for(i = 0; i < Size(pPH->orderQue); i++)
			PrintOrder((Order*)ObserveItem(pPH->orderQue, i),pPH->currentTime, pDebugFile, 1);
	}

	if(pPH->orderReceiver.progressingOrder != NULL)
	{
		PrintOrder(pPH->orderReceiver.progressingOrder,pPH->currentTime, pDebugFile, 1);	
	}

	if(Size(pPH->calculateQue)!=0)
	{
		for(i = 0; i < Size(pPH->calculateQue); i++)
			PrintOrder((Order*)ObserveItem(pPH->calculateQue, i),pPH->currentTime, pDebugFile, 1);
	}

	if(pPH->calculater.progressingOrder != NULL)
	{
		PrintOrder(pPH->calculater.progressingOrder,pPH->currentTime, pDebugFile, 1);
	}

	if(Size(pPH->preparationQue)!=0)
	{
		for(i = 0; i < Size(pPH->preparationQue); i++)
			PrintOrder((Order*)ObserveItem(pPH->preparationQue, i),pPH->currentTime, pDebugFile, 1);
	}

	if(Size(pPH->waitingPizzaQue)!=0)
	{
		for(i = 0; i < Size(pPH->waitingPizzaQue); i++)
			PrintOrder((Order*)ObserveItem(pPH->waitingPizzaQue, i),pPH->currentTime, pDebugFile, 1);
	}

	if(Size(pPH->deliveryQue)!=0)
	{
		for(i = 0; i < Size(pPH->deliveryQue); i++)
			PrintOrder((Order*)ObserveItem(pPH->deliveryQue, i),pPH->currentTime, pDebugFile, 1);
	}

	if(pPH->deliveryStaff.progressingOrder != NULL)
	{
		PrintOrder(pPH->deliveryStaff.progressingOrder,pPH->currentTime, pDebugFile, 1);
	}

	return;
}
