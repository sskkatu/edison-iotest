TEST = test
LIBS = libDevices.so
LIBSRCS += oled_ssd1306.cpp
LIBSRCS += glcdfont.cpp

OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(LIBSRCS:%.cpp=%.o))
DEPDIR = .dep
DEPENDS = $(addprefix $(DEPDIR)/, $(LIBSRCS:%.cpp=%.d))

.SUFFIXES : .d .o .cpp
.PHONY : all clean depend setsuid

CPP = g++
CPPFLAGS =-std=c++11 -Wall -O2 -I/usr/local/include

all : $(LIBS) $(TEST)

depend : $(DEPENDS)

clean :
	rm -f $(LIBS) $(TEST) *~ #*#
	rm -rf $(OBJDIR) $(DEPDIR)

$(TEST) : $(LIBS) test.o
	$(CPP) $(CPPFLAGS) -c test.cpp 
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
