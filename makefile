PROGRAM = ioTest
OBJS = main.o i2ctest.o gpiotest.o spitest.o

CC = gcc
CFLAGS = -Wall -O2

.SUFFICES : .o .c

$(PROGRAM) : $(OBJS)
	gcc $^ -o $(PROGRAM) -lmraa

.c.o : 
	$(CC) $(CFLAFGS) -c $<

.PHONY: clean
clean: 
	rm -f $(OBJS) $(PROGRAM) *~

main.o i2ctest.o spiotest.o spitest.o : iotest.h
