#include <stdio.h>
#include "List.h"
#include "PizzaHouse.h"
#include "FileReader.h"

int main()
{
	List* list;
	int i,j;
	int quantum = OrderListReader(&list,"testinput");
	//printf("hi\n");
	printf("size : %d\n",Size(list));
	//printf("hi\n");
	for(i = 0; i < Size(list); ++i)
	{
		Order* temp = ObserveItem(list,i);
		printf("Order Number %d\n", i);
		printf("PizzaList :\n");
		for(j = 0; j < Size(temp->pizzaList); ++j)
		{
			Pizza* tempPizza = ObserveItem(temp->pizzaList,j);
			printf("%d ",tempPizza->type);
		} 
		printf("\n");
	}
	return 0;
}