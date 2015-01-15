TEST = test
LIBS = libDevices.so

# Display Device
LIBSRCS += glcdfont.cpp
LIBSRCS += oled_ssd1306.cpp
LIBSRCS += lcd_ILI9340.cpp

# Sensor Device
LIBSRCS += accelerometer.cpp
LIBSRCS += barometer.cpp
LIBSRCS += compass.cpp
LIBSRCS += gps.cpp

OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(LIBSRCS:%.cpp=%.o))
DEPDIR = .dep
DEPENDS = $(addprefix $(DEPDIR)/, $(LIBSRCS:%.cpp=%.d))

.SUFFIXES : .d .o .cpp
.PHONY : all clean depend setsuid

CPP = g++
CPPFLAGS = -std=c++11 -Wall -O2 -I/usr/local/include

all : $(LIBS) $(TEST)

depend : $(DEPENDS)

clean :
	rm -f $(LIBS) $(TEST) test.o *~ \#*\#
	rm -rf $(OBJDIR) $(DEPDIR)

$(TEST) : $(LIBS) test.o
	$(CPP) $(OBJS) test.o -o $(TEST) -lmraa

$(LIBS) : $(OBJS)
	$(CPP) -shared $^ -o $(LIBS)

$(OBJDIR)/%.o : %.cpp
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(DEPDIR)/%.d : %.cpp
	@[ -d $(DEPDIR) ] || mkdir -p $(DEPDIR)
	@$(CPP) -M $(CPPFLAGS) -o $@.tmp -c $<
	@sed '1s/^/$(OBJDIR)\//' $@.tmp > $@
	@rm $@.tmp

-include $(DEPENDS)
