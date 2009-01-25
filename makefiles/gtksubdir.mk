include $(ROOTDIR)/makefiles/config.mk

INCLUDE = $(ZINCLUDE) $(EXTERNAL_INCLUDE) $(GTKINCLUDE)

HEADERS = $(wildcard *.h)
CSOURCES = $(wildcard *.c)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.c, %.o, $(CSOURCES))
OBJECTS += $(patsubst %.cpp, %.o, $(SOURCES))

.SUFFIXES: .c .cpp .o .h

.c.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

.cpp.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

all: $(OBJECTS)

clean:
	@$(RM) *.o *.d

-include *.d
