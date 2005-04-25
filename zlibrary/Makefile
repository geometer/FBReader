ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = libzlibrary.a

SUBDIRS_ALL = abstract/time abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml abstract/screenSize desktop/options desktop/dialogs unix/filesystem palm/pseudostl palm/options palm/filesystem palm/dialogs
QTSUBDIRS_ALL = qtopia/dialogs qtopia/options qtopia/view qtopia/screenSize qt/dialogs qt/screenSize
GTKSUBDIRS_ALL = gtk/dialogs gtk/screenSize

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml abstract/screenSize unix/filesystem
	QTSUBDIRS = qtopia/dialogs qtopia/options qtopia/view qtopia/screenSize
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/options desktop/dialogs abstract/screenSize unix/filesystem
	QTSUBDIRS = qt/dialogs qt/screenSize
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/options desktop/dialogs abstract/screenSize unix/filesystem
	QTSUBDIRS = qt/dialogs qt/screenSize
endif

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	SUBDIRS = abstract/time abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/options desktop/dialogs abstract/screenSize unix/filesystem
	GTKSUBDIRS = gtk/dialogs gtk/screenSize
endif

ifeq "$(TARGET_ARCH)" "palm"
	SUBDIRS = palm/pseudostl abstract/options abstract/util abstract/filesystem abstract/dialogs abstract/xml palm/options palm/filesystem palm/dialogs
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
