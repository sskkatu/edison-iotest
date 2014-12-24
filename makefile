PROGRAM = ioTest
OBJS = main.o i2ctest.o gpiotest.o

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

main.o : iotest.h
i2ctest.o : iotest.h
gpiotest.o : iotest.h
