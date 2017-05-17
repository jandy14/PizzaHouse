// -----------------------------------------------------------------------
// NAME : Jung Sung-wook	User ID : 20123307
// DUE DATE : 05/17/2017
// PROJECT #1
// FILE NAME : FileReader.c
// PROGRAM PURPOSE :
//	FileReader.h에 선언된 함수를 정의 합니다.
// -----------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FileReader.h"	//It just for type
#include "PizzaHouse.h"	//It just for type
#include "List.h"

// -----------------------------------------------------------------------
// FUNCTION OrderListReader :
//	Read File and parsing data
//	make order list and return quantum
// PARAMETER USAGE :
//	pOrderList : 함수의 결과물로 만들어질 리스트의 값을 넣어 줍니다.
//	pInputFile : 읽을 inputfile의 이름이 저장되어있습니다.
// FUNCTION CALLED :
//	in here
//		ParsingTextToOrder()
//	in <stdio.h>
//		printf(), fgets(), feof(), fopen(), fclose()
//	in <stdlib.h>
//		atoi()
//	in <string.h>
//		strtok()
//	in "List.h"
//		NewList(), Insert(), Size()
// -----------------------------------------------------------------------
int OrderListReader(List** pOrderList, char* pInputFile)
{
	char buf[1024];
	int quantum;
	//Make List
	List* orderList = NewList();
	FILE* f = fopen(pInputFile,"rt");
	if(f == NULL)
	{
		printf("InputFile Read Error\n");
		exit(0);
	}

	//Get quantum
	fgets(buf,1024,f);
	strtok(buf,"=");
	quantum = atoi(strtok(NULL," \n\r"));
	//second line text (useless)
	fgets(buf,1024,f);
	//파일이 끝날때까지 라인별로 읽어 order구조체에 데이터를 넣어 orderList에 저장
	while(!feof(f))
	{
		if(fgets(buf,1024,f)==NULL)
			break;
		if(buf[0] == '\n')
			break;
		Insert(orderList,(void*)ParsingTextToOrder(buf),Size(orderList));
	}
	//주소를 전해줌
	*pOrderList = orderList;
	fclose(f);
	//quantum은 반환
	return quantum;
}

// -----------------------------------------------------------------------
// FUNCTION ParsingTextToOrder :
//	Receive string and parsing to Order structure
// PARAMETER USAGE :
//	pBuf : 주문내용을 담고 있는 문자열
// -----------------------------------------------------------------------
Order* ParsingTextToOrder(char* pBuf)
{
	Order* item = (Order*)malloc(sizeof(Order));

	item->num = atoi(strtok(pBuf," "));
	item->arrivalTime = HourToSec(strtok(NULL," "));
	item->type = TextToOrderType(strtok(NULL," "));
	item->state = BeforeOrder;
	List* pizzaList = NewList();
	int serviceTime = 0;
	
	//make pizza list
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

	return item;
}

// -----------------------------------------------------------------------
// FUNCTION HourToSec :
//	hour format string to seconds 
// PARAMETER USAGE :
//	pHour : hour format string
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION TextToOrderType :
//	Parsing string to OrderType
// PARAMETER USAGE :
//	pOrderType : 파싱할 문자열
// -----------------------------------------------------------------------
OrderType TextToOrderType(char* pOrderType)
{
	if(strcmp(pOrderType,"phone") == 0)
		return PHONE;
	else
		return VISIT;

	printf("TextToOrderType Error\n");
	return VISIT;
}

// -----------------------------------------------------------------------
// FUNCTION TextToPizzaType :
//	Parsing string to PizzaType
// PARAMETER USAGE :
//	pPizzaType : 파싱할 문자열
// -----------------------------------------------------------------------
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

// -----------------------------------------------------------------------
// FUNCTION PizzaRemainingTime :
//	 pizza type to second
// PARAMETER USAGE :
//	pType : 만드는 시간이 알고싶은 피자타입
// -----------------------------------------------------------------------
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