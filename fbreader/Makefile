ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = $(UILIBS) -lm -L$(ZDIR) -lzlibrary $(EXPATLIBS) $(ENCALIBS) $(BZIP2LIBS)

TARGET = FBReader

ALL_SUBDIRS = common common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/pdb common/formats/txt common/formats/tcr common/formats/chm common/formats/oeb common/formats/dummy common/textview common/hyphenation common/fbreader palm desktop pdaxrom
ALL_ARCHSUBDIRS = palm desktop pdaxrom opie zaurus maemo
ALL_QSUBDIRS = zaurus/qtopia pdaxrom/qt desktop/qt opie
ALL_GTKSUBDIRS = pdaxrom/gtk desktop/gtk maemo

SUBDIRS = common common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/html common/formats/pdb common/formats/txt common/formats/tcr common/formats/chm common/formats/oeb common/formats/dummy common/textview common/hyphenation common/fbreader

PLATFORM_SUBDIR = $(TARGET_ARCH)

GTKPLATFORMS = "gtk maemo"
ifeq "$(UI_TYPE)" "$(findstring $(UI_TYPE), $(GTKPLATFORMS))"
	GTKSUBDIRS = $(TARGET_ARCH)/$(UI_TYPE)
endif

QTPLATFORMS = "qt qtopia opie"
ifeq "$(UI_TYPE)" "$(findstring $(UI_TYPE), $(QTPLATFORMS))"
	QSUBDIRS = $(TARGET_ARCH)/$(UI_TYPE)
endif

all:
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@for subdir in $(QSUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk; then \
			exit 1; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find . -name *.o` $(LIBS)
	@echo " OK"

install: all .really-install

.really-install:
	@cd $(TARGET_ARCH); make install 

package:
	@cd $(TARGET_ARCH); make all 

clean:
	@for subdir in $(ALL_SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for subdir in $(ALL_ARCHSUBDIRS); do \
		cd $$subdir; make clean; cd ..; \
	done;
	@for subdir in $(ALL_QSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk clean; \
	done;
	@for subdir in $(ALL_GTKSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk clean; \
	done;
	@$(RM) $(TARGET) err
