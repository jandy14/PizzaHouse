#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FileReader.h"
#include "PizzaHouse.h"
#include "List.h"

int OrderListReader(List** pOrderList, char* pInputFile)
{
	char buf[1024];
	int quantum;
	List* orderList = NewList();
	FILE* f = fopen(pInputFile,"rt");
	if(f == NULL)
	{
		printf("File Read Error\n");
		exit(0);
	}

	fscanf(f,"Q = %d\n",&quantum);
	fgets(buf,1024,f);
	while(!feof(f))
	{
		if(fgets(buf,1024,f)==NULL)
			break;
		if(buf[0] == '\n')
			break;
		printf("%s",buf);		//debug
		Insert(orderList,(void*)PhaseTextToOrder(buf),Size(orderList));
	}
	*pOrderList = orderList;
	return quantum;
}

Order* PhaseTextToOrder(char* pBuf)
{
	Order* item = (Order*)malloc(sizeof(Order));

	item->num = atoi(strtok(pBuf," "));
	item->arrivalTime = HourToSec(strtok(NULL," "));
	item->type = TextToOrderType(strtok(NULL," "));

	List* pizzaList = NewList();
	int serviceTime = 0;
	while(1)
	{
		char* token = strtok(NULL,",\n\r");
		if(token == NULL)
			break;
		Pizza* itemPizza = (Pizza*)malloc(sizeof(Pizza));
		itemPizza->order = item;
		itemPizza->type = TextToPizzaType(token);
		itemPizza->remainingTime = PizzaRemainingTime(itemPizza->type);
		serviceTime += itemPizza->remainingTime;
		Insert(pizzaList,(void*)itemPizza,Size(pizzaList));
	}
	item->pizzaList = pizzaList;
	item->serviceTime = serviceTime;
	item->timer = 0;

	return item;
}

int HourToSec(char* pHour)
{
	char time[3];
	int hour;
	int minute;

	time[0] = pHour[0];
	time[1] = pHour[1];
	hour = atoi(time);
	time[0] = pHour[3];
	time[1] = pHour[4];
	minute = atoi(time);

	return ((hour * 60) + minute) * 60;
}
OrderType TextToOrderType(char* pOrderType)
{
	if(strcmp(pOrderType,"phone") == 0)
		return PHONE;
	else
		return VISIT;

	printf("TextToOrderType Error\n");
	return VISIT;
}
PizzaType TextToPizzaType(char* pPizzaType)
{
	if(strcmp(pPizzaType,"cheese pizza") == 0)
		return CHEESE;
	else if(strcmp(pPizzaType,"pepperoni pizza") == 0)
		return PEPPERONI;
	else if(strcmp(pPizzaType,"potato pizza") == 0)
		return POTATO;
	else
		return SHRIMP;

	printf("TextToPizzaType Error\n");
	return CHEESE;
}
int PizzaRemainingTime(PizzaType pType)
{
	switch(pType)
	{
	case CHEESE:
		return CHEESE_TIME;
	case PEPPERONI:
		return PEPPERONI_TIME;
	case POTATO:
		return POTATO_TIME;
	default:
		return SHRIMP_TIME;
	}
	printf("PizzaRemainingTime Error\n");
	return CHEESE_TIME;
}