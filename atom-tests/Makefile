ROOTDIR = $(CURDIR)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = tests
target = tests

SUBDIRS = src src/constants src/network/atom

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk CFLAGS="$(CFLAGS) -fexceptions"; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find -L src -name *.o` -L$(LIBDIR) -lzlcore -lzltext -lz -lsqlite3
	@echo " OK"

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@$(RM) $(TARGET) err* tmp*
	@$(RM) *.log gmon*
	@$(RM) `find -L -name '*~'`
	@$(RM) `find -L -name '*.o'`
	@$(RM) `find -L -name '*.d'`

do_install:

