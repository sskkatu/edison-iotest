PROGRAM = i2ctest
OBJS = i2ctest.o 

CC = gcc
CFLAGS = -Wall -O2

.SUFFICES : .o .c

$(PROGRAM) : $(OBJS)
	gcc $^ -o $(PROGRAM) -lmraa

.c.o : 
	$(CC) $(CFLAFGS) -c $<

.PHONY: clean
clean: 
	rm $(OBJS) $(PROGRAM)

