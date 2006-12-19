ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat VERSION)

TARGET = libzlibrary-$(UI_TYPE).so.$(VERSION)
TARGET_SHORTNAME = libzlibrary-$(UI_TYPE).so

SUBDIRS_ALL = abstract/library abstract/message abstract/runnable abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/optionEntries abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/application abstract/view abstract/image abstract/xmlconfig desktop/dialogs unix/filesystem unix/time
QTSUBDIRS_ALL = qt/time qtopia/message qtopia/application qtopia/dialogs qtopia/view qt/dialogs qt/view qt/image qt/util opie/view opie/dialogs opie/application qt/application-desktop qt/application-pdaxrom opie/library qt/library-desktop qt/library-pdaxrom qtopia/library
GTKSUBDIRS_ALL = gtk/time gtk/dialogs gtk/view-desktop gtk/view-pdaxrom gtk/image gtk/util maemo/dialogs maemo/view gpe/dialogs gpe/view gtk/application-desktop gtk/application-pdaxrom maemo/application-maemo maemo/application-maemo2 gpe/application maemo/library gtk/library-desktop gtk/library-pdaxrom gpe/library

SUBDIRS = abstract/library abstract/message abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/optionEntries abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image abstract/xmlconfig abstract/application abstract/view unix/filesystem unix/time

ifeq "$(UI_TYPE)" "qt"
  SUBDIRS += desktop/dialogs
  QTSUBDIRS = qt/time qt/dialogs qt/view qt/image qt/util
ifeq "$(TARGET_ARCH)" "desktop"
  QTSUBDIRS += qt/library-desktop qt/application-desktop
else                             
  QTSUBDIRS += qt/library-pdaxrom qt/application-pdaxrom
endif
endif

ifeq "$(UI_TYPE)" "gtk"
  SUBDIRS += desktop/dialogs
  GTKSUBDIRS = gtk/time gtk/dialogs gtk/image gtk/util
ifeq "$(TARGET_ARCH)" "desktop"
  GTKSUBDIRS += gtk/library-desktop gtk/view-desktop gtk/application-desktop
else
  GTKSUBDIRS += gtk/library-pdaxrom gtk/view-pdaxrom gtk/application-pdaxrom
endif
endif

ifeq "$(UI_TYPE)" "gpe"
  GTKSUBDIRS = gpe/library gtk/time gpe/dialogs gpe/application gtk/image gtk/util gpe/view
endif

ifeq "$(UI_TYPE)" "opie"
  QTSUBDIRS = opie/library qt/time opie/dialogs opie/application opie/view qt/image qt/util
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <qtopia> <qtopia-240x320> <qtopia-640x480>)"
  QTSUBDIRS = qtopia/message qtopia/library qt/time qtopia/application qtopia/dialogs qtopia/view qt/util qt/image
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <maemo> <maemo2>)"
  GTKSUBDIRS = gtk/time maemo/dialogs maemo/view gtk/image gtk/util maemo/application-$(UI_TYPE) maemo/library
endif

.objects:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;
	@for subdir in $(QTSUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;

$(TARGET): .objects
	@echo -n "Creating $@ ..."
	@$(RM_QUIET) $(TARGET)
#	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS))
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS)) $(UILIBS) $(ENCALIBS) $(EXPATLIBS) $(BZIP2LIBS)
	@ln -sf $(TARGET) $(TARGET_SHORTNAME)
	@echo " OK"

SHARE_ZLIBRARY = $(DESTDIR)$(SHAREDIR)/zlibrary

ENCODING_FILES = $(wildcard data/encodings/*)
ifeq "$(TARGET_ARCH)" "maemo"
  ENCODING_FILES = $(filter-out %/Big5,$(wildcard data/encodings/*))
endif

do_install:
	@install -d $(SHARE_ZLIBRARY)
	@install -m 0644 data/keynames.$(TARGET_ARCH)-$(UI_TYPE).xml $(SHARE_ZLIBRARY)/keynames.xml
	@install -d $(SHARE_ZLIBRARY)/encodings
	@install -m 0644 $(ENCODING_FILES) $(SHARE_ZLIBRARY)/encodings
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)

clean:
	@for subdir in $(SUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@for subdir in $(QTSUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk clean; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.*
