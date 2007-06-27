ROOTDIR = $(CURDIR)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	LIBS = -lm -L../zlibrary/core -lzlcore $(XML_LIB) $(ARCHIVER_LIB) -ldl
else
	LIBS = -lm -L../zlibrary/platform -lzlplatform-$(UI_TYPE) -L../zlibrary/core -lzlcore $(UILIBS) $(XML_LIB) $(ARCHIVER_LIB)
endif

TARGET = GeometricCalculator

SUBDIRS = src src/document src/io src/model src/modelDecorator src/polynom src/translator src/ui src/calculator
ARCHSUBDIRS = zaurus maemo

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

GC_SHAREDIR = $(DESTDIR)$(SHAREDIR)/$(TARGET)

do_install:
	@install -d $(DESTDIR)$(BINDIR)
	@install $(TARGET) $(DESTDIR)$(BINDIR)
	@install -d $(GC_SHAREDIR)/default
	@install -m 0644 data/default/config.$(TARGET_ARCH).xml $(GC_SHAREDIR)/default/config.xml
	@install -m 0644 data/default/keymap.$(TARGET_ARCH).xml $(GC_SHAREDIR)/default/keymap.xml
	@install -d $(GC_SHAREDIR)/samples
	@install -m 0644 share/samples/*.tar $(GC_SHAREDIR)/samples
	@install -d $(GC_SHAREDIR)/resources
	@install -m 0644 share/resources/*.xml $(GC_SHAREDIR)/resources

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
