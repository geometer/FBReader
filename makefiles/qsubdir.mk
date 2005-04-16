include $(ROOTDIR)/makefiles/config.mk
include moc.mk

INCLUDE = $(QTINCLUDE) $(ZINCLUDE) $(EXTERNALINCLUDE)

HEADERS = $(wildcard *.h)
SOURCES =	$(wildcard *.cpp)
OBJMOC = $(patsubst %.cpp, %.o, $(SRCMOC))
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES))

DEPFILE = .depend

.dep:
	@echo -n "Calculating dependencies ..."
	@$(RM_QUIET) $(DEPFILE)
	@if [ "$(SOURCES)" != "" ]; then \
		$(DEPGEN) $(INCLUDE) $(SOURCES) >> $(DEPFILE); \
	fi;
	@echo " OK"

.SUFFIXES: .cpp .moc.cpp .moc.o .o .h

.cpp.o:
	@echo -n "Compiling $@ ..."
	@$(CC) -c $(CFLAGS) $(INCLUDE) $<
	@echo " OK"

.h.moc.cpp:
	@echo -n "Generating $@ ..."
	@$(MOC) $< -o $@
	@echo " OK"

all: .dep $(OBJECTS) $(OBJMOC)

clean:
	@$(RM) $(DEPFILE) *.o *.moc.cpp

ifneq "$(wildcard $(DEPFILE))" ""
  include $(DEPFILE)
endif
