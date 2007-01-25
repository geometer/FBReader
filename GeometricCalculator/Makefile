ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = -lm -L../zlibrary -lzlibrary-$(UI_TYPE) $(EXPATLIBS) $(ENCALIBS) $(BZIP2LIBS) $(UILIBS)

TARGET = GeometricCalculator

SUBDIRS = src/document src/io src/model src/modelDecorator src/polynom src/translator src/ui src/calculator
ARCHSUBDIRS = zaurus maemo

all: resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` $(LIBS)
	@echo " OK"

resources:
ifneq "$(RESOURCE_COMPILER)" ""
	@echo -n "Creating resource object..."
	@echo "ApplicationName ICON icons/win32/$(TARGET).ico" > $(TARGET).rc
	@for file in icons/win32/$(TARGET)/*.bmp; do \
	   echo `basename $$file .bmp` BITMAP $$file >> $(TARGET).rc; \
	 done
	@$(RESOURCE_COMPILER) $(TARGET).rc -o src/$(TARGET)_rc.o
	@$(RM_QUIET) $(TARGET).rc
	@echo " OK"
endif

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
