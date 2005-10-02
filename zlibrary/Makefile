ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = libzlibrary.a

SUBDIRS_ALL = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/xml/own abstract/deviceInfo abstract/view abstract/image desktop/options desktop/dialogs unix/filesystem palm/pseudostl palm/options palm/filesystem palm/dialogs palm/deviceInfo unix/time palm/time palm/view
QTSUBDIRS_ALL = qtopia/dialogs qtopia/options qtopia/scrollableview qtopia/deviceInfo qtopia/view qt/dialogs qt/deviceInfo qt/view qt/image opie/view
GTKSUBDIRS_ALL = gtk/dialogs gtk/deviceInfo gtk/view-desktop gtk/view-pdaxrom gtk/image maemo/dialogs maemo/deviceInfo maemo/view

ifeq "$(TARGET_ARCH)" "zaurus-qtopia"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/deviceInfo abstract/view unix/filesystem unix/time abstract/image desktop/options
	QTSUBDIRS = qtopia/dialogs qtopia/scrollableview qtopia/deviceInfo qtopia/view qtopia/options qt/image
endif

ifeq "$(TARGET_ARCH)" "qvfb-opie"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/deviceInfo abstract/view unix/filesystem unix/time abstract/image desktop/options
	QTSUBDIRS = qtopia/dialogs qtopia/scrollableview qtopia/deviceInfo opie/view qtopia/options qt/image
endif

ifeq "$(TARGET_ARCH)" "opensimpad-opie"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/deviceInfo abstract/view unix/filesystem unix/time abstract/image desktop/options
	QTSUBDIRS = qtopia/dialogs qtopia/scrollableview qtopia/deviceInfo opie/view qtopia/options qt/image
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-qt"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
	QTSUBDIRS = qt/dialogs qt/deviceInfo qt/view qt/image
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-gtk"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
	GTKSUBDIRS = gtk/dialogs gtk/deviceInfo gtk/view-pdaxrom gtk/image
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
	QTSUBDIRS = qt/dialogs qt/deviceInfo qt/view qt/image
endif

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
	GTKSUBDIRS = gtk/dialogs gtk/deviceInfo gtk/view-desktop gtk/image
endif

ifeq "$(TARGET_ARCH)" "maemo"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
	GTKSUBDIRS = maemo/dialogs maemo/deviceInfo maemo/view gtk/image
endif

ifeq "$(TARGET_ARCH)" "palm"
	SUBDIRS = palm/pseudostl abstract/options abstract/util abstract/xml abstract/xml/own abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/dialogs abstract/deviceInfo abstract/view abstract/image palm/options palm/filesystem palm/dialogs palm/deviceInfo abstract/time palm/time palm/view
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
