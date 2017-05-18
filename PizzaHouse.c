// -----------------------------------------------------------------------
// NAME : Jung Sung-wook	User ID : 20123307
// DUE DATE : 05/17/2017
// PROJECT #1
// FILE NAME : PizzaHouse.c
// PROGRAM PURPOSE :
//	PizzaHouse.h에 선언된 함수를 정의 합니다.
// -----------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include "PizzaHouse.h"
#include "List.h"

// -----------------------------------------------------------------------
// FUNCTION PizzaHouseOpen :
//	PizzaHouse객체를 생성하고 값을 초기화 그리고 반환
// PARAMETER USAGE :
//	pOrderList : 주문이 저장되어있는 리스트의 헤더
//	pQuantum : quantum값
//	pScheduling : 스케줄링 종류(FCFS, SJF, RR)
//	pDestination : 결과를 출력할 스트림 보통은 stdout
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION RefreshWorker :
//	Worker의 값을 초기화
// PARAMETER USAGE :
//	pTarget : 값을 초기화할 대상의 주소
// -----------------------------------------------------------------------
void RefreshWorker(Worker* pTarget)
{
	pTarget->progressingOrder = NULL;
	pTarget->remainingTime = 0;
}

// -----------------------------------------------------------------------
// FUNCTION RefreshWorker :
//	PizzaMaker의 값을 초기화
// PARAMETER USAGE :
//	pTarget : 값을 초기화할 대상의 주소
//	pIsAllClear : 리스트까지 모두 초기화 할것인가
// -----------------------------------------------------------------------
void RefreshPizzaMaker(PizzaMaker* pTarget, int pIsAllClear)
{
	pTarget->progressingPizza = NULL;
	pTarget->remainingTime = 0;
	if(pIsAllClear)
		DeleteList(pTarget->workQue);
}

// -----------------------------------------------------------------------
// FUNCTION GoNextQue :
//	Order객체를 받아서 다음 큐로 이동
// PARAMETER USAGE :
//	pOrder : 이동할 객체
//	pDestinationQue : 이동할 목적지 큐
//	pScheduling : 스케줄링값에 따라 큐의 순서가 바뀔수 있다
// -----------------------------------------------------------------------
void GoNextQue(Order* pOrder, List* pDestinationQue, Scheduling pScheduling)
{
	int index = 0;
	++(pOrder->state);
	//SJF인 경우 큐에 넣을때 serviceTime에 따라 큐의 순서 변동
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

// -----------------------------------------------------------------------
// FUNCTION FindNextEventTime :
//	다음 이벤트 타임이 언제인지 비교하는 함수
// PARAMETER USAGE :
//	pPH : 다음 이벤트 시간이 저장되어 있는 PizzaHouse
//	pTime : 비교할 시간
// -----------------------------------------------------------------------
void FindNextEventTime(PizzaHouse* pPH, int pTime)
{
	if(pPH->nextEventTime == -1)
		pPH->nextEventTime = pTime;
	else if(pPH->nextEventTime > pTime)
		pPH->nextEventTime = pTime;

	//printf("%d vs %d\n", pPH->nextEventTime, pTime);

	return;
}
// -----------------------------------------------------------------------
// FUNCTION OrderStart :
//	주문 리스트에 있는 값중 현재시각과 같은 주문은 주문큐로 이동시킨다.
// PARAMETER USAGE :
//	pPH : 주문리스트나 다음 목적지를 가지고 있는 PizzaHouse객체
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION ReceiveOrder :
//	orderReceiver가 해야하는 일을 처리한다.
//	현재 처리중인 주문이 끝났는지 확인하고 끝났으면 다음 큐(calculateQue)로 이동
//	일이 없으면 (방금 처리한거 포함) 본인의 큐(orderQue)에서 값을 들고온다.
//	없다면 쉰다.
// PARAMETER USAGE :
//	pPH : orderReceiver가 저장되어있는 PizzaHouse객체
// -----------------------------------------------------------------------
void ReceiveOrder(PizzaHouse* pPH)
{
	//worker has job.
	if(pPH->orderReceiver.progressingOrder)	
	{
		pPH->orderReceiver.remainingTime -= pPH->elapsedTime;
		//worker finish job.
		if(pPH->orderReceiver.remainingTime == 0)
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
	//worker hasn't job and remain job in Queue.
	if(Size(pPH->orderQue) != 0 && pPH->orderReceiver.progressingOrder == NULL)
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

// -----------------------------------------------------------------------
// FUNCTION CalculateOrder :
//	calculator가 해야하는 일을 처리한다.
//	현재 처리중인 주문이 끝났는지 확인하고 끝났으면 다음 큐(preparationQue)로 이동
//	일이 없으면 (방금 처리한거 포함) 본인의 큐(calculator)에서 값을 들고온다.
//	없다면 쉰다.
// PARAMETER USAGE :
//	pPH : calculator가 저장되어있는 PizzaHouse객체
// -----------------------------------------------------------------------
void CalculateOrder(PizzaHouse* pPH)
{
	//worker has job.
	if(pPH->calculater.progressingOrder)
	{
		pPH->calculater.remainingTime -= pPH->elapsedTime;
		//worker finish job.
		if(pPH->calculater.remainingTime == 0) 
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
	//worker hasn't job and remain job in Queue.
	if(Size(pPH->calculateQue) != 0 && pPH->calculater.progressingOrder == NULL)
	{
		pPH->calculater.progressingOrder = (Order*)Pop(pPH->calculateQue);
		++(pPH->calculater.progressingOrder->state);
		pPH->calculater.remainingTime = CALCULATE_TIME;
		FindNextEventTime(pPH, CALCULATE_TIME);
	}
	return;
}
// -----------------------------------------------------------------------
// FUNCTION MakePizza :
//	pizzaMaker가 해야하는 일을 처리한다.
//	
// PARAMETER USAGE :
//	pPH : pizzaMaker가 저장되어있는 PizzaHouse객체
// -----------------------------------------------------------------------
void MakePizza(PizzaHouse* pPH)
{
	//When RR, pizza should be assigned to the pizzaMaker
	if(pPH->scheduling == RR)
		DistributeWork(pPH);

	int i;
	//pizzaMaker loop
	for(i = 0; i < PIZZA_MAKER_AMOUNT; i++)
	{
		//pizzaMaker has job.
		if(pPH->pizzaMaker[i].progressingPizza)
		{
			//pizza가 완성까지 남은 시간
			pPH->pizzaMaker[i].progressingPizza->remainingTime -= pPH->elapsedTime;
			//pizzaMaker가 다음일을 할때까지 남은 시간(RR일때는 값이 다를수 있다)
			pPH->pizzaMaker[i].remainingTime -= pPH->elapsedTime;

			//pizzaMaker가 다음일을 해야할 시간이 되었다.
			if(pPH->pizzaMaker[i].remainingTime == 0)
			{
				//pizza가 완성된 것이라면
				if(pPH->pizzaMaker[i].progressingPizza->remainingTime == 0)
				{
					RefreshPizzaMaker(&(pPH->pizzaMaker[i]),0);
				}
				//pizza가 완성된 것이 아니라면
				else
				{
					Insert(pPH->pizzaMaker[i].workQue,(void*)pPH->pizzaMaker[i].progressingPizza,Size(pPH->pizzaMaker[i].workQue));
					RefreshPizzaMaker(&(pPH->pizzaMaker[i]),0);
				}
			}
			else
				FindNextEventTime(pPH, pPH->pizzaMaker[i].remainingTime);
		}
		//When RR
		if(pPH->scheduling == RR)
		{
			//pizzaMaker doesn't need or Queue doesn't remain next job.
			if(Size(pPH->pizzaMaker[i].workQue) == 0 || pPH->pizzaMaker[i].progressingPizza != NULL)
				continue;
			//if pizzaMaker need next job and remain next job.
			pPH->pizzaMaker[i].progressingPizza = (Pizza*)Pop(pPH->pizzaMaker[i].workQue);
			//Set pizzaMaker's remainingTime that maximun is quantum value.
			if(pPH->quantum > pPH->pizzaMaker[i].progressingPizza->remainingTime)
				pPH->pizzaMaker[i].remainingTime = pPH->pizzaMaker[i].progressingPizza->remainingTime;
			else
				pPH->pizzaMaker[i].remainingTime = pPH->quantum;
			FindNextEventTime(pPH, pPH->pizzaMaker[i].remainingTime);
		}
		//When FCFS, SJF
		else
		{
			//worker hasn't job and remain job in Queue.
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
// -----------------------------------------------------------------------
// FUNCTION DistributeWork :
//	pizza를 pizzaMaker에게 골고루 나눠준다.
//	
// PARAMETER USAGE :
//	pPH : preparationQue와 pizzaMaker가 저장된 PizzaHouse객체
// -----------------------------------------------------------------------
void DistributeWork(PizzaHouse* pPH)
{
	static int index = 0;
	//모든 preparationQue에 있는 주문들의 피자를 makingPizzaQue에 넣는다.
	while(1)
	{
		Order* item;
		if((item = (Order*)Pop(pPH->preparationQue)) == NULL)
			break;
		ReadyPizza(item, pPH->makingPizzaQue, pPH->waitingPizzaQue);
	}

	//모든 makingPizzaQue에 있는 pizza를 pizzaMaker에게 순차적으로 나누어 준다.
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

// -----------------------------------------------------------------------
// FUNCTION ReadyPizza :
//	주문을 받아 pizza를 pizzaList에 넣고 주문은 pNextQue에 넣어준다.
//	
// PARAMETER USAGE :
//	pOrder : 처리할 주문
//	pPizzaList : 주문에서 나온 피자를 넣을 큐
//	pNextQue : 처리한 주문을 보내줄 큐
// -----------------------------------------------------------------------
void ReadyPizza(Order* pOrder, List* pPizzaList ,List* pNextQue)
{
	int i;
	for(i = 0; i < Size(pOrder->pizzaList); ++i)
		Insert(pPizzaList, ObserveItem(pOrder->pizzaList, i), Size(pPizzaList));
	GoNextQue(pOrder, pNextQue, FCFS);
	return;
}

// -----------------------------------------------------------------------
// FUNCTION PrintOrder :
//	주문을 출력해주는 함수
//	포맷은 number arrivalTime orderType serviceTime turnaroundTime
//	주문이 NULL이면 num을 초기화한다.
// PARAMETER USAGE :
//	pOrder : 처리할 주문
//	pCurrentTime : 현재시각
//	pOutput : 출력할 스트림 NULL이면 stdout
//	pIsTest : 완성전 테스트용 출력인지
// -----------------------------------------------------------------------
void PrintOrder(Order* pOrder, int pCurrentTime, FILE* pOutput, int pIsTest)
{
	static int num = 1;

	//num 초기화
	if(pOrder == NULL)
	{
		num = 1;
		return;
	}
	//pOutput default set
	if(pOutput == NULL)
		pOutput = stdout;


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

// -----------------------------------------------------------------------
// FUNCTION CheckCompletedOrder :
//	waitingPizzaQue에 저장된 주문중 모든 피자가 완성된 경우
//	주문의 타입을 기준으로 다음 행동을 한다.
// PARAMETER USAGE :
//	pPH : 처리할 데이터가 들어있는 PizzaHouse객체
// -----------------------------------------------------------------------
void CheckCompletedOrder(PizzaHouse* pPH)
{
	int listSize = Size(pPH->waitingPizzaQue);
	int i,j;

	//리스트의 모든 주문 확인
	for(i = 0; i < listSize; ++i)
	{
		int isAllComplete = 1;
		//주문을 하나 꺼낸다
		Order* order = (Order*)Pop(pPH->waitingPizzaQue);
		int pizzaListSize = Size(order->pizzaList);
		//모든 피자가 완성됐는지 확인
		for(j = 0; j < pizzaListSize; ++j)
		{
			Pizza* pizza = (Pizza*)ObserveItem(order->pizzaList,j);
			if(pizza->remainingTime)
			{
				isAllComplete = 0;
				break;
			}
		}
		//모두 완성이라면
		if(isAllComplete)
		{
			//방문고객이면 완료
			if(order->type == VISIT)
			{
				order->state = Complete;
				PrintOrder(order,pPH->currentTime,pPH->destination,0);
				FreeOrderMemory(order);
				++(pPH->completeCount);
			}
			//phone이면 deliveryQue로
			else
				GoNextQue(order, pPH->deliveryQue, FCFS);
		}
		//모두 완성이 아니면 다시 waitingPizzaQue에 다시 넣어줌
		else
			Insert(pPH->waitingPizzaQue,(void*)order,Size(pPH->waitingPizzaQue));
	}
}

// -----------------------------------------------------------------------
// FUNCTION FreeOrderMemory :
//	Order객체에 할달된 메모리 해제
// PARAMETER USAGE :
//	pOrder : 할당을 풀어줄 Order객체
// -----------------------------------------------------------------------
void FreeOrderMemory(Order* pOrder)
{
	DeleteList(pOrder->pizzaList);
	free(pOrder);
}

// -----------------------------------------------------------------------
// FUNCTION Delivery :
//	deliveryStaff가 해야하는 일을 처리한다.
//	현재 처리중인 주문이 끝났는지 확인하고 끝났으면 출력!
//	일이 없으면 (방금 처리한거 포함) 본인의 큐(orderQue)에서 값을 들고온다.
//	없다면 쉰다.
// PARAMETER USAGE :
//	pPH : deliveryStaff가 저장되어있는 PizzaHouse객체
// -----------------------------------------------------------------------
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
// -----------------------------------------------------------------------
// FUNCTION PizzaHouseRun :
//	pizzaHouse의 현재시각 이벤트를 처리하고 다음 이벤트 시각을 찾습니다.
// PARAMETER USAGE :
//	pPH : 이벤트를 진행할 PizzaHouse
// -----------------------------------------------------------------------
BOOL PizzaHouseRun(PizzaHouse* pPH)
{
	//현재시각 갱신
	if(pPH->nextEventTime == -1)
		pPH->elapsedTime = 0;
	else
		pPH->elapsedTime = pPH->nextEventTime;
	pPH->currentTime += pPH->elapsedTime;
	pPH->nextEventTime = -1;

	//현재시각에 맞는 주문이 있으면 처리
	OrderStart(pPH);
	//orderReceiver's Work
	ReceiveOrder(pPH);
	//calculator's Work
	CalculateOrder(pPH);
	//pizzaMaker's Work
	MakePizza(pPH);
	//check completed order and do next job
	CheckCompletedOrder(pPH);
	//deliveryStaff's Work
	Delivery(pPH);

	return !(IsAllOrderClear(pPH));
}

// -----------------------------------------------------------------------
// FUNCTION IsAllOrderClear :
//	모든 주문이 완성됐는지 확인하고 반환
// PARAMETER USAGE :
//	pPH : 확인이 필요한 PizzaHouse 객체
// -----------------------------------------------------------------------
BOOL IsAllOrderClear(PizzaHouse* pPH)
{
	return pPH->orderCount == pPH->completeCount? TRUE:FALSE;
}

// -----------------------------------------------------------------------
// FUNCTION PizzaHouseClose :
//	pizzaHouse 정리 (메모리 해제)
// PARAMETER USAGE :
//	pPH : 정리하려는 PizzaHouse 객체
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION PizzaHouseNowState :
//	pizzaHouse의 현재 상태에 대해 출력해주는 함수
// PARAMETER USAGE :
//	pPH : 상태를 확인하고 싶은 pizzaHouse
//	pDebugFile : 출력 스트림
// -----------------------------------------------------------------------
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
