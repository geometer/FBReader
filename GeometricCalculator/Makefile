ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = $(UILIBS) -lm -L../zlibrary -lzlibrary-$(UI_TYPE) $(EXPATLIBS) $(ENCALIBS) $(BZIP2LIBS)

TARGET = GeometricCalculator

SUBDIRS = src/document src/io src/model src/modelDecorator src/options src/polynom src/translator src/ui src/calculator
ARCHSUBDIRS = zaurus maemo

all:
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
	@install -d $(DESTDIR)$(SHAREDIR)/$(TARGET)
	@install -d $(DESTDIR)$(SHAREDIR)/$(TARGET)/default
	@install -m 0644 data/default/config.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/$(TARGET)/default/config.xml
	@install -m 0644 data/default/keymap.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/$(TARGET)/default/keymap.xml

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
