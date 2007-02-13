ROOTDIR = $(PWD)/../..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = -lm -L$(ROOTDIR)/zlibrary -lzlibrary-$(UI_TYPE) $(EXPATLIBS) $(ENCALIBS) $(BZIP2LIBS) $(UILIBS)

TARGET = SampleApplication

SUBDIRS = src
ARCHSUBDIRS = maemo

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` $(LIBS)
	@echo " OK"

install: all do_install

do_install:
	@install -d $(DESTDIR)$(BINDIR)
	@install $(TARGET) $(DESTDIR)$(BINDIR)

package:
	@if [ -d $(TARGET_ARCH) ]; then \
		cd $(TARGET_ARCH); $(MAKE); \
	fi;

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for target in $(ARCHSUBDIRS); do \
		if [ -d $$target ]; then \
			$(MAKE) -C $$target clean; \
		fi; \
	done;
	@$(RM) $(TARGET) err
