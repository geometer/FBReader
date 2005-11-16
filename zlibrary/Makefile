ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

TARGET = libzlibrary.a

SUBDIRS_ALL = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/xml/own abstract/encoding abstract/deviceInfo abstract/view abstract/image desktop/options desktop/dialogs unix/filesystem palm/pseudostl palm/options palm/filesystem palm/dialogs palm/deviceInfo unix/time palm/time palm/view
QTSUBDIRS_ALL = qtopia/dialogs qtopia/options qtopia/scrollableview qtopia/deviceInfo qtopia/view qtopia/util qt/dialogs qt/deviceInfo qt/view qt/image qt/util opie/view opie-opensimpad/util opie-openzaurus/util
GTKSUBDIRS_ALL = gtk/dialogs gtk/deviceInfo gtk/view-desktop gtk/view-pdaxrom gtk/image gtk/util maemo/dialogs maemo/deviceInfo maemo/view maemo/util

SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image desktop/options desktop/dialogs abstract/deviceInfo abstract/view unix/filesystem unix/time
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
ifeq "$(UI_TYPE)" "opie"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/xml abstract/xml/expat abstract/encoding abstract/image desktop/options abstract/deviceInfo abstract/view unix/filesystem unix/time
	QTSUBDIRS = qtopia/dialogs qtopia/scrollableview qtopia/deviceInfo opie/view qtopia/options qt/image
ifeq "$(TARGET_ARCH)" "opensimpad-0.9.0"
	QTSUBDIRS += opie-opensimpad/util
else
	QTSUBDIRS += opie-openzaurus/util
endif
endif
ifeq "$(UI_TYPE)" "qtopia"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/filesystem/zip abstract/filesystem/tar abstract/options abstract/util abstract/scrollableview abstract/xml abstract/xml/expat abstract/encoding abstract/deviceInfo abstract/view unix/filesystem unix/time abstract/image desktop/options
	QTSUBDIRS = qtopia/dialogs qtopia/scrollableview qtopia/deviceInfo qtopia/view qtopia/options qtopia/util qt/image
endif
ifeq "$(UI_TYPE)" "maemo"
	GTKSUBDIRS = maemo/dialogs maemo/deviceInfo maemo/view gtk/image maemo/util
endif
ifeq "$(UI_TYPE)" "palm"
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
