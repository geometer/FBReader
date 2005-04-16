include $(ROOTDIR)/makefiles/config.mk

INCLUDE = $(ZINCLUDE) $(EXTERNALINCLUDE)

HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

DEPFILE = .depend

.dep:
	@echo -n "Calculating dependencies ..."
	@$(RM_QUIET) $(DEPFILE)
	@if [ "$(SOURCES)" != "" ]; then \
		$(DEPGEN) $(INCLUDE) $(SOURCES) >> $(DEPFILE); \
	fi;
	@echo " OK"

.SUFFIXES: .cpp .o .h

.cpp.o:
	@echo -n "Compiling $@ ..."
	@$(CC) -c $(CFLAGS) $(INCLUDE) $<
	@echo " OK"

all: .dep $(OBJECTS)

clean:
	@$(RM) $(DEPFILE) *.o

ifneq "$(wildcard $(DEPFILE))" ""
  include $(DEPFILE)
endif
