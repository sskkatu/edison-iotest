PROGRAM = ioTest
SRCS = main.c i2ctest.c gpiotest.c spi_LCDtest.c spi_WS2812Btest.c i2c_OLEDtest.c glcdfont.c
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:%.c=%.o))
DEPDIR = .dep
DEPENDS = $(addprefix $(DEPDIR)/, $(SRCS:%.c=%.d))

.SUFFIXES : .d .o .c
.PHONY : all clean depend

CC = gcc
CFLAGS = -Wall -O2 -I/usr/include/ImageMagick/  -I/usr/local/include

all : $(PROGRAM)

depend : $(DEPENDS)

clean :
	rm -f $(PROGRAM) *~ #*#
	rm -rf $(OBJDIR) $(DEPDIR)

$(PROGRAM) : $(OBJS)
	gcc $^ -o $(PROGRAM) -lmraa -lMagickWand

$(OBJDIR)/%.o : %.c
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(DEPDIR)/%.d : %.c
	@[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	@$(CC) -M $(CFLAGS) -o $@.tmp -c $<
	@sed '1s/^/$(OBJDIR)\//' $@.tmp > $@
	@rm $@.tmp

-include $(DEPENDS)
