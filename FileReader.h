#ifndef __FILEREADER_H__
#define __FILEREADER_H__
#include "PizzaHouse.h"
#include "List.h"

int OrderListReader(List** orderList, char* inputFile);
Order* ParsingTextToOrder(char* buf);
int HourToSec(char* hour);
OrderType TextToOrderType(char* orderType);
PizzaType TextToPizzaType(char* pizzaType);
int PizzaRemainingTime(PizzaType type);
#endif
