ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

TARGET = libzlibrary.a

SUBDIRS_ALL = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/deviceInfo abstract/application abstract/view abstract/image abstract/xmlconfig desktop/dialogs unix/filesystem unix/time
QTSUBDIRS_ALL = qtopia/dialogs qtopia/options qtopia/deviceInfo qtopia/view qtopia/util qt/dialogs qt/deviceInfo qt/view qt/image qt/util opie/view opie-opensimpad/util opie-openzaurus/util
GTKSUBDIRS_ALL = gtk/dialogs gtk/deviceInfo gtk/view-desktop gtk/view-pdaxrom gtk/image gtk/util maemo/dialogs maemo/deviceInfo maemo/view maemo/util gpe/dialogs

SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image abstract/xmlconfig desktop/dialogs abstract/deviceInfo abstract/application abstract/view unix/filesystem unix/time

ifeq "$(UI_TYPE)" "qt"
	QTSUBDIRS = qt/dialogs qt/deviceInfo qt/view qt/image qt/util
endif

ifeq "$(UI_TYPE)" "gtk"
	GTKSUBDIRS = gtk/dialogs gtk/deviceInfo gtk/image gtk/util
ifeq "$(TARGET_ARCH)" "desktop"
	GTKSUBDIRS += gtk/view-desktop
else
	GTKSUBDIRS += gtk/view-pdaxrom
endif
endif

ifeq "$(UI_TYPE)" "gpe"
	GTKSUBDIRS = gpe/dialogs gtk/deviceInfo gtk/image gtk/util maemo/view
  SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image abstract/xmlconfig abstract/deviceInfo abstract/application abstract/view unix/filesystem unix/time
endif

ifeq "$(UI_TYPE)" "opie"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image abstract/xmlconfig abstract/deviceInfo abstract/application abstract/view unix/filesystem unix/time
	QTSUBDIRS = qtopia/dialogs qtopia/deviceInfo opie/view qtopia/options qt/image
ifeq "$(TARGET_ARCH)" "opensimpad-0.9.0"
	QTSUBDIRS += opie-opensimpad/util
else
	QTSUBDIRS += opie-openzaurus/util
endif
endif

ifeq "$(UI_TYPE)" "qtopia"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/bzip2 abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/deviceInfo abstract/application abstract/view unix/filesystem unix/time abstract/image abstract/xmlconfig
	QTSUBDIRS = qtopia/dialogs qtopia/deviceInfo qtopia/view qtopia/options qtopia/util qt/image
endif

ifeq "$(UI_TYPE)" "$(findstring $(UI_TYPE), maemo maemo2)"
	GTKSUBDIRS = maemo/dialogs maemo/deviceInfo maemo/view gtk/image maemo/util
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
	@$(AR) $(TARGET) `find . -name *.o`
	@echo " OK"

install: $(TARGET)

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
	@$(RM) $(TARGET)
