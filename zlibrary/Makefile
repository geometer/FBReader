ROOTDIR = $(PWD)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = libzlibrary.a

SUBDIRS_ALL = abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/xml desktop/dialogs
QTSUBDIRS_ALL = qtopia/dialogs qtopia/options qtopia/view qt/dialogs
GTKSUBDIRS_ALL = gtk/dialogs

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	SUBDIRS = abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml
	QTSUBDIRS = qtopia/dialogs qtopia/options qtopia/view
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom"
	SUBDIRS = abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/xml desktop/dialogs
	QTSUBDIRS = qt/dialogs
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	SUBDIRS = abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/xml desktop/dialogs
	QTSUBDIRS = qt/dialogs
endif

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	SUBDIRS = abstract/dialogs abstract/filesystem abstract/options abstract/util abstract/view abstract/xml desktop/xml desktop/dialogs
	GTKSUBDIRS = gtk/dialogs
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
	@$(AR) $(TARGET) */*/*.o
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
