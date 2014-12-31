PROGRAM = ioTest
OBJS = main.o i2ctest.o gpiotest.o spitest.o spi-ws2812Btest.o

CC = gcc
CFLAGS = -Wall -O2 -I/usr/include/ImageMagick/ 

.SUFFICES : .o .c

$(PROGRAM) : $(OBJS)
	gcc $^ -o $(PROGRAM) -lmraa -lMagickWand

.c.o : 
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: 
	rm -f $(OBJS) $(PROGRAM) *~

main.o i2ctest.o spiotest.o spitest.o spi-ws2812Btest.o : iotest.h
