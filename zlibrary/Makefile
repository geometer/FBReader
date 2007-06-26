ROOTDIR = $(CURDIR)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat VERSION)

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  TARGET = libzlibrary-$(UI_TYPE).so.$(VERSION)
  TARGET_SHORTNAME = libzlibrary-$(UI_TYPE).so
else
  TARGET = libzlibrary-$(UI_TYPE).a
endif

SUBDIRS_ALL = src/abstract/library src/abstract/message src/abstract/runnable src/abstract/time src/abstract/filesystem src/abstract/filesystem/zip src/abstract/filesystem/bzip2 src/abstract/filesystem/tar src/abstract/options src/abstract/util src/abstract/xml src/abstract/xml/expat src/abstract/encoding src/abstract/application src/abstract/view src/abstract/image src/abstract/xmlconfig src/desktop/application src/desktop/dialogs src/unix/filesystem src/unix/time src/unix/iconv src/win32/library src/win32/encoding src/win32/view src/win32/filesystem src/win32/time src/win32/w32widgets src/win32/dialogs src/win32/application src/win32/image src/win32/config src/win32/message src/posix/filesystem src/unix/message src/abstract/resources src/unix/library
QTSUBDIRS_ALL = src/qt/time src/qtopia/message src/qtopia/application src/qtopia/dialogs src/qtopia/view src/qt/dialogs src/qt/view src/qt/image src/qt/util src/opie/view src/opie/dialogs src/opie/application src/qt/application-desktop src/qt/application-pdaxrom src/opie/library src/qt/library-desktop src/qt/library-pdaxrom src/qtopia/library src/qt/filesystem src/qt4/time src/qt4/dialogs src/qt4/view src/qt4/image src/qt4/util src/qt4/application src/qt4/library src/qt4/filesystem
GTKSUBDIRS_ALL = src/gtk/time src/gtk/dialogs src/gtk/view src/gtk/image src/gtk/util src/maemo/dialogs src/maemo/view src/gpe/dialogs src/gpe/view src/gtk/application-desktop src/gtk/application-pdaxrom src/maemo/application-maemo src/maemo/application-maemo2 src/gpe/application src/maemo/library src/gtk/library-desktop src/gtk/library-pdaxrom src/gpe/library src/gtk/filesystem src/maemo/message src/gtk/pixbuf
ARCHSUBDIRS = zaurus maemo

SUBDIRS = src/abstract/library src/abstract/message src/abstract/time src/abstract/filesystem src/abstract/filesystem/zip src/abstract/filesystem/bzip2 src/abstract/filesystem/tar src/abstract/options src/abstract/util src/abstract/xml src/abstract/xml/expat src/abstract/encoding src/abstract/image src/abstract/application src/abstract/view src/posix/filesystem src/unix/time src/abstract/resources

ifeq "$(TARGET_ARCH)" "desktop"
  SUBDIRS += src/desktop/application
endif

ifeq "$(UI_TYPE)" "qt"
  SUBDIRS += src/desktop/dialogs src/unix/message
  QTSUBDIRS = src/qt/time src/qt/dialogs src/qt/view src/qt/image src/qt/util src/qt/filesystem src/qt/library-$(TARGET_ARCH) src/qt/application-$(TARGET_ARCH)
endif

ifeq "$(UI_TYPE)" "qt4"
  SUBDIRS += src/desktop/dialogs src/unix/message
  QTSUBDIRS = src/qt4/time src/qt4/dialogs src/qt4/view src/qt4/image src/qt4/util src/qt4/filesystem src/qt4/library src/qt4/application
endif

ifeq "$(UI_TYPE)" "gtk"
  SUBDIRS += src/desktop/dialogs src/unix/message
  GTKSUBDIRS = src/gtk/time src/gtk/dialogs src/gtk/image src/gtk/util src/gtk/filesystem src/gtk/library-$(TARGET_ARCH) src/gtk/view src/gtk/application-$(TARGET_ARCH) src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "gpe"
  SUBDIRS += src/unix/message
  GTKSUBDIRS = src/gpe/library src/gtk/time src/gpe/dialogs src/gpe/application src/gtk/image src/gtk/util src/gpe/view src/gtk/filesystem src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "opie"
  SUBDIRS += src/unix/message
  QTSUBDIRS = src/opie/library src/qt/time src/opie/dialogs src/opie/application src/opie/view src/qt/image src/qt/util src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <qtopia> <qtopia-240x320> <qtopia-640x480>)"
  QTSUBDIRS = src/qtopia/message src/qtopia/library src/qt/time src/qtopia/application src/qtopia/dialogs src/qtopia/view src/qt/util src/qt/image src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <maemo> <maemo2>)"
  GTKSUBDIRS = src/gtk/time src/maemo/dialogs src/maemo/view src/gtk/image src/gtk/util src/maemo/application-$(UI_TYPE) src/maemo/library src/gtk/filesystem src/maemo/message src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "win32"
  SUBDIRS += src/desktop/application src/desktop/dialogs src/win32/library src/win32/view src/win32/filesystem src/win32/time src/win32/w32widgets src/win32/dialogs src/win32/application src/win32/image src/win32/config src/win32/message src/win32/encoding
else
  SUBDIRS += src/abstract/xmlconfig src/unix/filesystem src/unix/iconv src/unix/library
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
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS)) $(UILIBS) $(XML_LIB) $(ARCHIVER_LIB)
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

ifeq "$(TARGET_ARCH)" "desktop"
  KEYNAMES = keynames-$(UI_TYPE).xml
else
  KEYNAMES = keynames.xml
endif

do_install:
	@install -d $(SHARE_ZLIBRARY)
	@install -m 0644 data/keynames.$(TARGET_ARCH)-$(UI_TYPE).xml $(SHARE_ZLIBRARY)/$(KEYNAMES)
	@install -d $(SHARE_ZLIBRARY)/encodings
	@install -m 0644 $(ENCODING_FILES) $(SHARE_ZLIBRARY)/encodings
	@install -d $(SHARE_ZLIBRARY)/resources
	@install -m 0644 $(wildcard data/resources/*.xml) $(SHARE_ZLIBRARY)/resources
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif
	@$(MAKE) -C core do_install
	@$(MAKE) -C text do_install

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
