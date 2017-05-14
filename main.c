#include <stdio.h>
#include "List.h"
#include "PizzaHouse.h"
#include "FileReader.h"

int main()
{
	List* list;
	//int i,j;
	int quantum = OrderListReader(&list,"testinput");

	// list = NewList();
	// Insert(list,(void*)"1",0);
	// printf("size : %d\n", Size(list));
	// Insert(list,(void*)"1",0);
	// printf("size : %d\n", Size(list));
	// Insert(list,(void*)"1",0);
	// printf("size : %d\n", Size(list));
	// Insert(list,(void*)"1",0);
	// printf("size : %d\n", Size(list));
	// Insert(list,(void*)"1",0);
	// printf("size : %d\n", Size(list));

	// Pop(list);
	// printf("size : %d\n", Size(list));

	// Pop(list);
	// printf("size : %d\n", Size(list));

	// Pop(list);
	// printf("size : %d\n", Size(list));

	// Pop(list);
	// printf("size : %d\n", Size(list));

	// //printf("hi\n");
	// printf("size : %d\n",Size(list));
	// //printf("hi\n");
	// for(i = 0; i < Size(list); ++i)
	// {
	// 	Order* temp = ObserveItem(list,i);
	// 	printf("Order Number %d\n", i);
	// 	printf("PizzaList :\n");
	// 	for(j = 0; j < Size(temp->pizzaList); ++j)
	// 	{
	// 		Pizza* tempPizza = ObserveItem(temp->pizzaList,j);
	// 		printf("%d ",tempPizza->type);
	// 	} 
	// 	printf("\n");
	// }
	// PrintOrder((Order*)ObserveItem(list,0),0,1);
	// PrintOrder((Order*)ObserveItem(list,1),0,1);
	// int i;
	// printf("size : %d\n", Size(list));
	// for(i = 0; i < 2; i++)
	// {
	// 	printf("pop!\n");
	// 	Order* order = (Order*)Pop(list);
	// 	printf("size : %d\n", Size(list));
	// }

	PizzaHouse* pizzaHouse;
	pizzaHouse = PizzaHouseOpen(list, quantum, FCFS);
	while(PizzaHouseRun(pizzaHouse));
	PizzaHouseClose(pizzaHouse);
	quantum = OrderListReader(&list,"testinput");
	pizzaHouse = PizzaHouseOpen(list, quantum, SJF);
	while(PizzaHouseRun(pizzaHouse));
	PizzaHouseClose(pizzaHouse);
	quantum = OrderListReader(&list,"testinput");
	pizzaHouse = PizzaHouseOpen(list, quantum, RR);
	while(PizzaHouseRun(pizzaHouse));
	PizzaHouseClose(pizzaHouse);
	return 0;
}
