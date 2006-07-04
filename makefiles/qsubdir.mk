include $(ROOTDIR)/makefiles/config.mk
-include moc.mk

INCLUDE = $(QTINCLUDE) $(ZINCLUDE) $(EXTERNALINCLUDE)

HEADERS = $(wildcard *.h)
SOURCES =	$(wildcard *.cpp)
OBJMOC = $(patsubst %.cpp, %.o, $(SRCMOC))
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

.SUFFIXES: .cpp .moc.cpp .moc.o .o .h

.cpp.o:
	@echo -n "Compiling $@ ..."
	@$(CC) -MMD -c $(CFLAGS) $(INCLUDE) $<
	@echo " OK"

.h.moc.cpp:
	@echo -n "Generating $@ ..."
	@$(MOC) $< -o $@
	@echo " OK"

all: $(OBJECTS) $(OBJMOC)

clean:
	@$(RM) *.o *.moc.cpp *.d

-include *.d
