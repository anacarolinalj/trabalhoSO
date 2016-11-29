RM := rm -f
CC := g++
CFLAGS := -g
LDLIBS := -lm

SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:%.cpp=%.o)
DEPS := $(SOURCES:%.cpp=%.d)
BINARY := pseudoSO
FILES_TO_CLEAN := $(OBJECTS) $(DEPS)

.PHONY : all clean realclean

all : $(BINARY) clean

clean :
	$(RM) $(FILES_TO_CLEAN)

realclean : FILES_TO_CLEAN += $(BINARY)
realclean : clean

-include $(DEPS)

$(OBJECTS) : %.o : %.cpp
	$(CC) $(CFLAGS) -c -MMD -o $@ $<

$(BINARY) : $(OBJECTS)
	$(CC) -o $@ $^ $(LDLIBS)
