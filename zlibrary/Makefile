ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  CFLAGS += -fPIC
endif

VERSION = $(shell cat VERSION)

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  TARGET = libzlibrary-$(UI_TYPE).so.$(VERSION)
  TARGET_SHORTNAME = libzlibrary-$(UI_TYPE).so
else
  TARGET = libzlibrary-$(UI_TYPE).a
endif

SUBDIRS_ALL = src/abstract/library src/abstract/message src/abstract/runnable src/abstract/time src/abstract/dialogs src/abstract/filesystem src/abstract/filesystem/zip src/abstract/filesystem/bzip2 src/abstract/filesystem/tar src/abstract/options src/abstract/optionEntries src/abstract/util src/abstract/xml src/abstract/xml/expat src/abstract/encoding src/abstract/application src/abstract/view src/abstract/image src/abstract/xmlconfig src/desktop/dialogs src/unix/filesystem src/unix/time src/win32/library src/win32/view src/win32/filesystem src/win32/time src/win32/dialogs src/win32/application
QTSUBDIRS_ALL = src/qt/time src/qtopia/message src/qtopia/application src/qtopia/dialogs src/qtopia/view src/qt/dialogs src/qt/view src/qt/image src/qt/util src/opie/view src/opie/dialogs src/opie/application src/qt/application-desktop src/qt/application-pdaxrom src/opie/library src/qt/library-desktop src/qt/library-pdaxrom src/qtopia/library src/qt/filesystem
GTKSUBDIRS_ALL = src/gtk/time src/gtk/dialogs src/gtk/view-desktop src/gtk/view-pdaxrom src/gtk/image src/gtk/util src/maemo/dialogs src/maemo/view src/gpe/dialogs src/gpe/view src/gtk/application-desktop src/gtk/application-pdaxrom src/maemo/application-maemo src/maemo/application-maemo2 src/gpe/application src/maemo/library src/gtk/library-desktop src/gtk/library-pdaxrom src/gpe/library src/gtk/filesystem
ARCHSUBDIRS = zaurus maemo

SUBDIRS = src/abstract/library src/abstract/message src/abstract/time src/abstract/dialogs src/abstract/filesystem src/abstract/filesystem/zip src/abstract/filesystem/bzip2 src/abstract/filesystem/tar src/abstract/options src/abstract/optionEntries src/abstract/util src/abstract/xml src/abstract/xml/expat src/abstract/encoding src/abstract/image src/abstract/xmlconfig src/abstract/application src/abstract/view src/unix/filesystem src/unix/time

ifeq "$(UI_TYPE)" "qt"
  SUBDIRS += src/desktop/dialogs
  QTSUBDIRS = src/qt/time src/qt/dialogs src/qt/view src/qt/image src/qt/util src/qt/filesystem src/qt/library-$(TARGET_ARCH) src/qt/application-$(TARGET_ARCH)
endif

ifeq "$(UI_TYPE)" "gtk"
  SUBDIRS += src/desktop/dialogs
  GTKSUBDIRS = src/gtk/time src/gtk/dialogs src/gtk/image src/gtk/util src/gtk/filesystem src/gtk/library-$(TARGET_ARCH) src/gtk/view-$(TARGET_ARCH) src/gtk/application-$(TARGET_ARCH)
endif

ifeq "$(UI_TYPE)" "gpe"
  GTKSUBDIRS = src/gpe/library src/gtk/time src/gpe/dialogs src/gpe/application src/gtk/image src/gtk/util src/gpe/view src/gtk/filesystem
endif

ifeq "$(UI_TYPE)" "opie"
  QTSUBDIRS = src/opie/library src/qt/time src/opie/dialogs src/opie/application src/opie/view src/qt/image src/qt/util src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <qtopia> <qtopia-240x320> <qtopia-640x480>)"
  QTSUBDIRS = src/qtopia/message src/qtopia/library src/qt/time src/qtopia/application src/qtopia/dialogs src/qtopia/view src/qt/util src/qt/image src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <maemo> <maemo2>)"
  GTKSUBDIRS = src/gtk/time src/maemo/dialogs src/maemo/view src/gtk/image src/gtk/util src/maemo/application-$(UI_TYPE) src/maemo/library src/gtk/filesystem
endif

ifeq "$(UI_TYPE)" "win32"
  SUBDIRS += src/win32/library src/win32/view src/win32/filesystem src/win32/time src/win32/dialogs src/win32/application
endif

.objects:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;
	@for subdir in $(QTSUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;

$(TARGET): .objects
	@echo -n "Creating $@ ..."
	@$(RM_QUIET) $(TARGET)
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS)) $(UILIBS) $(ENCALIBS) $(EXPATLIBS) $(BZIP2LIBS)
	@ln -sf $(TARGET) $(TARGET_SHORTNAME)
else
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS))
endif
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

package:
	@if [ -d $(TARGET_ARCH) ]; then \
		cd $(TARGET_ARCH); $(MAKE); \
	fi;

clean:
	@for subdir in $(SUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@for subdir in $(QTSUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk clean; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk clean; \
		fi; \
	done;
	@for target in $(ARCHSUBDIRS); do \
		$(MAKE) -C $$target clean; \
	done;
	@$(RM) *.so *.so.* *.a
