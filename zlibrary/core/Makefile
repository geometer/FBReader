ROOTDIR = $(CURDIR)/../..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat ../VERSION)
SOVERSION = $(shell cat ../SOVERSION)

ifeq "$(ZLSHARED)" "yes"
  TARGET = libzlcore.so.$(VERSION)
  TARGET_SONAME = libzlcore.so.$(SOVERSION)
  TARGET_SHORTNAME = libzlcore.so
else
  TARGET = libzlcore.a
endif

SUBDIRS_ALL = src/library src/util src/filesystem src/filesystem/zip src/filesystem/bzip2 src/filesystem/tar src/dialogs src/optionEntries src/application src/view src/splittedView src/encoding src/options src/message src/resources src/time src/xml src/xml/expat src/image src/language src/desktop/application src/desktop/dialogs src/posix/filesystem src/unix/time src/unix/xmlconfig src/unix/filesystem src/unix/iconv src/unix/message src/unix/library

SUBDIRS = src/library src/util src/filesystem src/filesystem/zip src/filesystem/bzip2 src/filesystem/tar src/dialogs src/optionEntries src/application src/view src/splittedView src/encoding src/options src/message src/resources src/time src/xml src/xml/expat src/image src/language src/posix/filesystem src/unix/time

ifeq "<$(TARGET_ARCH)>" "$(findstring <$(TARGET_ARCH)>, <desktop> <pepperpad3>)"
  SUBDIRS += src/desktop/application src/desktop/dialogs src/unix/message
endif

ifeq "$(TARGET_ARCH)" "pdaxrom"
  SUBDIRS += src/desktop/dialogs src/unix/message
endif

ifeq "<$(TARGET_ARCH)>" "$(findstring <$(TARGET_ARCH)>, <openzaurus> <opensimpad-0.9.0> <qvfb>)"
  SUBDIRS += src/unix/message
endif

ifeq "$(TARGET_ARCH)" "win32"
  SUBDIRS += src/desktop/application src/desktop/dialogs
else
  SUBDIRS += src/unix/xmlconfig src/unix/filesystem src/unix/iconv src/unix/library
endif

.objects:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;

$(TARGET): .objects
	@echo -n "Creating $@ ..."
	@$(RM_QUIET) $(TARGET)
ifeq "$(ZLSHARED)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET_SONAME) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS)) $(XML_LIB) $(ARCHIVER_LIB) -ldl
	@ln -sf $(TARGET) $(TARGET_SONAME)
	@ln -sf $(TARGET) $(TARGET_SHORTNAME)
else
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS))
endif
	@echo " OK"

SHARE_ZLIBRARY = $(DESTDIR)$(SHAREDIR)/zlibrary

ENCODING_FILES = $(wildcard data/encodings/*)
ifeq "$(TARGET_ARCH)" "maemo"
  ENCODING_FILES = $(filter-out %/Big5,$(wildcard data/encodings/*))
endif

do_install:
	@install -d $(SHARE_ZLIBRARY)
	@if [ -f data/messages.$(TARGET_ARCH).xml ]; then \
		install -m 0644 data/messages.$(TARGET_ARCH).xml $(SHARE_ZLIBRARY)/messages.xml; \
	fi
	@install -d $(SHARE_ZLIBRARY)/encodings
	@install -m 0644 $(ENCODING_FILES) $(SHARE_ZLIBRARY)/encodings
	@install -d $(SHARE_ZLIBRARY)/resources
	@install -m 0644 $(wildcard data/resources/*.xml) $(SHARE_ZLIBRARY)/resources
	@install -m 0644 $(wildcard data/languagePatterns.zip) $(SHARE_ZLIBRARY)
	@install -d $(SHARE_ZLIBRARY)/default
	@if [ -e data/default/config.$(TARGET_ARCH).xml ]; then \
		install -m 0644 data/default/config.$(TARGET_ARCH).xml $(SHARE_ZLIBRARY)/default/config.xml; \
	fi
	@if [ -e data/default/config.$(TARGET_ARCH)-$(UI_TYPE).xml ]; then \
		install -m 0644 data/default/config.$(TARGET_ARCH)-$(UI_TYPE).xml $(SHARE_ZLIBRARY)/default/config.xml; \
	fi
ifeq "$(ZLSHARED)" "yes"
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif

do_install_dev:
	@install -d $(DESTDIR)$(LIBDIR)
ifeq "$(ZLSHARED)" "yes"
	@ln -sf $(TARGET) $(DESTDIR)$(LIBDIR)/$(TARGET_SONAME)
	@ln -sf $(TARGET) $(DESTDIR)$(LIBDIR)/$(TARGET_SHORTNAME)
else
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif
	@install -d $(DESTDIR)$(INCDIR)/zlibrary/core
	@install -m 0644 $(wildcard include/*.h) $(DESTDIR)$(INCDIR)/zlibrary/core
	@install -d $(DESTDIR)$(INCDIR)/zlibrary/core/optionEntries
	@install -m 0644 $(wildcard include/optionEntries/*.h) $(DESTDIR)$(INCDIR)/zlibrary/core/optionEntries

clean:
	@for subdir in $(SUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.* *.a
