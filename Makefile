EXE = 20123307_proj1
OBJS = main.c FileReader.c List.c Option.c PizzaHouse.c

$(EXE) : $(OBJS)
	gcc -o $@ $^
clean :
	rm *.o $(EXE)