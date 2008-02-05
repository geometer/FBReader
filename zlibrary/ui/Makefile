ROOTDIR = $(CURDIR)/../..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat ../VERSION)

ifeq "$(ZLSHARED)" "yes"
  TARGET = zlui-$(UI_TYPE).so
else
  TARGET = libzlui-$(UI_TYPE).a
endif

SUBDIRS_ALL = src/win32/view src/win32/w32widgets src/win32/dialogs src/win32/application src/win32/image src/win32/library src/win32/time src/unix/message src/win32/message
QTSUBDIRS_ALL = src/qt/time src/qtopia/message src/qtopia/application src/qtopia/dialogs src/qtopia/optionView src/qtopia/view src/qt/dialogs src/qt/view src/qt/image src/qt/util src/opie/view src/opie/dialogs src/opie/application src/qt/application-desktop src/qt/application-pdaxrom src/opie/library src/qt/library src/qtopia/library src/qt/filesystem src/qt4/time src/qt4/dialogs src/qt4/view src/qt4/image src/qt4/util src/qt4/application src/qt4/library src/qt4/filesystem src/ezx/library src/ezx/application src/ezx/dialogs src/ezx/view src/ezx/optionView
GTKSUBDIRS_ALL = src/gtk/time src/gtk/dialogs src/gtk/optionView src/gtk/view src/gtk/image src/gtk/util src/maemo/dialogs src/maemo/optionView src/maemo/view src/gpe/dialogs src/gpe/view src/gtk/application-desktop src/gtk/application-pdaxrom src/maemo/application src/gpe/application src/maemo/library src/gtk/library src/gpe/library src/gtk/filesystem src/maemo/message src/gtk/pixbuf

SUBDIRS =

ifeq "$(UI_TYPE)" "qt"
  QTSUBDIRS = src/qt/time src/qt/dialogs src/qt/view src/qt/image src/qt/util src/qt/filesystem src/qt/library src/unix/message src/qt/application-$(TARGET_ARCH)
endif

ifeq "$(UI_TYPE)" "qt4"
  QTSUBDIRS = src/qt4/time src/qt4/dialogs src/qt4/view src/qt4/image src/qt4/util src/qt4/filesystem src/qt4/library src/unix/message src/qt4/application
endif

ifeq "$(UI_TYPE)" "gtk"
  GTKSUBDIRS = src/gtk/time src/gtk/dialogs src/gtk/optionView src/gtk/image src/gtk/util src/gtk/filesystem src/gtk/library src/gtk/view src/unix/message src/gtk/application-$(TARGET_ARCH) src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "gpe"
  GTKSUBDIRS = src/gpe/library src/gtk/time src/gpe/dialogs src/gpe/application src/gtk/image src/gtk/util src/gpe/view src/gtk/filesystem src/unix/message src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "opie"
  QTSUBDIRS = src/opie/library src/qt/time src/opie/dialogs src/opie/application src/opie/view src/qt/image src/qt/util src/unix/message src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <qtopia> <qtopia-240x320> <qtopia-640x480>)"
  QTSUBDIRS = src/qtopia/message src/qtopia/library src/qt/time src/qtopia/application src/qtopia/dialogs src/qtopia/optionView src/qtopia/view src/qt/util src/qt/image src/qt/filesystem
endif

ifeq "$(UI_TYPE)" "ezx"
  QTSUBDIRS = src/unix/message src/ezx/library src/qt/time src/ezx/application src/ezx/dialogs src/ezx/optionView src/ezx/view src/qt/util src/qt/image src/qt/filesystem
endif

ifeq "<$(UI_TYPE)>" "$(findstring <$(UI_TYPE)>, <maemo2> <maemo4>)"
  GTKSUBDIRS = src/gtk/time src/maemo/dialogs src/maemo/optionView src/maemo/view src/gtk/image src/gtk/util src/maemo/application src/maemo/library src/gtk/filesystem src/maemo/message src/gtk/pixbuf
endif

ifeq "$(UI_TYPE)" "win32"
  SUBDIRS += src/win32/view src/win32/w32widgets src/win32/dialogs src/win32/application src/win32/image src/win32/library src/win32/time src/win32/message
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
ifeq "$(ZLSHARED)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS)) $(UILIBS) $(CORE_LIBS)
else
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS) $(QTSUBDIRS) $(GTKSUBDIRS))
endif
	@echo " OK"

SHARE_ZLIBRARY = $(DESTDIR)$(SHAREDIR)/zlibrary
LIB_ZLIBRARY = $(DESTDIR)$(LIBDIR)/zlibrary

ifeq "<$(TARGET_ARCH)>" "$(findstring <$(TARGET_ARCH)>, <desktop> <pdaxrom>)"
  KEYNAMES = keynames-$(UI_TYPE).xml
else
  KEYNAMES = keynames.xml
endif

do_install:
	@install -d $(SHARE_ZLIBRARY)
	@if [ -f data/messages.$(TARGET_ARCH).xml ]; then \
		install -m 0644 data/messages.$(TARGET_ARCH).xml $(SHARE_ZLIBRARY)/messages.xml; \
	fi
	@install -m 0644 data/keynames.$(TARGET_ARCH)-$(UI_TYPE).xml $(SHARE_ZLIBRARY)/$(KEYNAMES)
ifeq "$(ZLSHARED)" "yes"
	@install -d $(LIB_ZLIBRARY)/ui
	@install $(TARGET) $(LIB_ZLIBRARY)/ui
endif

do_install_dev:

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
	@$(RM) *.so *.so.* *.a
