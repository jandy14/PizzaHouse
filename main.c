#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "PizzaHouse.h"
#include "FileReader.h"

int main(int argc, char** argv)
{
	char* debugFileName = NULL;
	char* outputFileName = NULL;
	char* inputFileName = NULL;
	FILE* debugFile = NULL;
	FILE* outputFile = NULL;

	if(argc < 2)
	{
		printf("Usage : %s InputFile [-d Filename] [-o FileName]\n", argv[0]);
		printf("\tInputFile     : InputFileName\n");
		printf("\t-d Filename   : Print Debug Log to File\n");
		printf("\t-o Filename   : Print Output to File\n");
		exit(0);
	}

	inputFileName = argv[1];
	ParsingOption(argc, argv, &debugFileName, &outputFileName);

	if(debugFileName != NULL)
	{
		debugFile = fopen(debugFileName, "wt");
	}
	if(outputFileName != NULL)
		outputFile = fopen(outputFileName, "wt");
	else
		outputFile = stdout;

	List* list;
	int quantum;
	PizzaHouse* pizzaHouse;

	fprintf(outputFile, "==============FCFS==============\n");
	if(debugFile)
		fprintf(debugFile, "==================FCFS==================\n");
	quantum = OrderListReader(&list, inputFileName);
	pizzaHouse = PizzaHouseOpen(list, quantum, FCFS, outputFile);
	while(PizzaHouseRun(pizzaHouse))
	{
		if(debugFile)
		{
			PizzaHouseNowState(pizzaHouse, debugFile);
		}
	}
	PizzaHouseClose(pizzaHouse);

	fprintf(outputFile, "===============SJF==============\n");
	if(debugFile)
		fprintf(debugFile, "===================SJF==================\n");
	quantum = OrderListReader(&list, inputFileName);
	pizzaHouse = PizzaHouseOpen(list, quantum, SJF, outputFile);
	while(PizzaHouseRun(pizzaHouse))
	{
		if(debugFile)
			PizzaHouseNowState(pizzaHouse, debugFile);
	}
	PizzaHouseClose(pizzaHouse);

	fprintf(outputFile, "===============RR===============\n");
	if(debugFile)
		fprintf(debugFile, "===================RR===================\n");
	quantum = OrderListReader(&list, inputFileName);
	pizzaHouse = PizzaHouseOpen(list, quantum, RR, outputFile);
	while(PizzaHouseRun(pizzaHouse))
	{
		if(debugFile)
			PizzaHouseNowState(pizzaHouse, debugFile);
	}
	PizzaHouseClose(pizzaHouse);

	if(debugFile)
		fclose(debugFile);
	if(outputFile)
		fclose(outputFile);
	return 0;
}
