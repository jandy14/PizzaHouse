// -----------------------------------------------------------------------
// NAME : Jung Sung-wook	User ID : 20123307
// DUE DATE : 05/17/2017
// PROJECT #1
// FILE NAME : main.c
// PROGRAM PURPOSE :
//	This program is pizzahouse simulator that apply process scheduling.
// -----------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "PizzaHouse.h"
#include "FileReader.h"
#include "Option.h"

// -----------------------------------------------------------------------
// FUNCTION main :
//	create pizzahouse and run and close about all kind of scheduling.
// PARAMETER USAGE :
//	argc : the number of strings pointed to by argv
//	argv : an array pointers to the string passed on the command line
// FUNCTION CALLED :
//	in <stdio.h>
//		printf(), fprintf(), fopen(), fclose()
//	in <stdlib.h>
//		exit()
//	in "List.h"
//		It just for "List" type
//	in "PizzaHouse.h"
//		PizzaHouseOpen(), PizzaHouseRun(),
//		PizzaHouseNowState(), PizzaHouseClose()
//	in "FileReader.h"
//		OrderListReader()
//	in "Option.h"
//		ParsingOption()
// -----------------------------------------------------------------------
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

	//Insert FileName to char*
	inputFileName = argv[1];
	//To Parse to debugFileName and outputFileName
	ParsingOption(argc, argv, &debugFileName, &outputFileName);

	//File Open if not NULL
	if(debugFileName != NULL)
		debugFile = fopen(debugFileName, "wt");
	if(outputFileName != NULL)
		outputFile = fopen(outputFileName, "wt");
	else
		outputFile = stdout;	//It is default

	List* list;		//temp list 
	int quantum;
	PizzaHouse* pizzaHouse;

	/*-----FCFS-----*/

	fprintf(outputFile, "==============FCFS==============\n");
	//DebugFile header
	if(debugFile)
		fprintf(debugFile, "==================FCFS==================\n");
	//read inputFile and parsing
	quantum = OrderListReader(&list, inputFileName);
	//create and initailization
	pizzaHouse = PizzaHouseOpen(list, quantum, FCFS, outputFile);
	//run pizzahouse about current time
	while(PizzaHouseRun(pizzaHouse))
	{
		//If you need to debug
		if(debugFile)
			PizzaHouseNowState(pizzaHouse, debugFile);
	}
	//clear memory
	PizzaHouseClose(pizzaHouse);

	/*------SJF------*/

	fprintf(outputFile, "===============SJF==============\n");
	//DebugFile header
	if(debugFile)
		fprintf(debugFile, "===================SJF==================\n");
	//read inputFile and parsing
	quantum = OrderListReader(&list, inputFileName);
	//create and initailization
	pizzaHouse = PizzaHouseOpen(list, quantum, SJF, outputFile);
	//run pizzahouse about current time
	while(PizzaHouseRun(pizzaHouse))
	{
		//If you need to debug
		if(debugFile)
			PizzaHouseNowState(pizzaHouse, debugFile);
	}
	//clear memory
	PizzaHouseClose(pizzaHouse);

	/*------RR------*/

	fprintf(outputFile, "===============RR===============\n");
	//DebugFile header
	if(debugFile)
		fprintf(debugFile, "===================RR===================\n");
	//read inputFile and parsing
	quantum = OrderListReader(&list, inputFileName);
	//create and initailization
	pizzaHouse = PizzaHouseOpen(list, quantum, RR, outputFile);
	//run pizzahouse about current time
	while(PizzaHouseRun(pizzaHouse))
	{
		//If you need to debug
		if(debugFile)
			PizzaHouseNowState(pizzaHouse, debugFile);
	}
	//clear memory
	PizzaHouseClose(pizzaHouse);

	//close file
	if(debugFile)
		fclose(debugFile);
	if(outputFile)
		fclose(outputFile);

	return 0;
}
