include $(ROOTDIR)/makefiles/config.mk

INCLUDE = $(ZINCLUDE) $(EXTERNAL_INCLUDE)

HEADERS = $(wildcard *.h)
SOURCES_CPP = $(wildcard *.cpp)
SOURCES_OBJCPP = $(wildcard *.M)
SOURCES_OBJC = $(wildcard *.m)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES_CPP)) $(patsubst %.M, %.o, $(SOURCES_OBJCPP)) $(patsubst %.m, %.o, $(SOURCES_OBJC))

.SUFFIXES: .cpp .M .m .o .h

.cpp.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

.M.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

.m.o:
	@echo -n 'Compiling $@ ...'
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo ' OK'

all: $(OBJECTS)

clean:
	@$(RM) *.o *.s *.ld *.d

-include *.d
