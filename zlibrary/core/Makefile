ROOTDIR = $(CURDIR)/../..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat ../VERSION)

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  TARGET = libzlcore.so.$(VERSION)
  TARGET_SHORTNAME = libzlcore.so
else
  TARGET = libzlcore.a
endif

SUBDIRS_ALL = src/library src/util src/filesystem src/filesystem/zip src/filesystem/bzip2 src/filesystem/tar src/dialogs src/optionEntries src/application src/view src/encoding src/options src/message src/resources src/time src/xml src/xml/expat src/image src/desktop/application src/desktop/dialogs src/posix/filesystem src/unix/time src/unix/xmlconfig src/unix/filesystem src/unix/iconv src/unix/message src/unix/library

SUBDIRS = src/library src/util src/filesystem src/filesystem/zip src/filesystem/bzip2 src/filesystem/tar src/dialogs src/optionEntries src/application src/view src/encoding src/options src/message src/resources src/time src/xml src/xml/expat src/image src/posix/filesystem src/unix/time

ifeq "$(TARGET_ARCH)" "desktop"
  SUBDIRS += src/desktop/application src/desktop/dialogs src/unix/message
endif

ifeq "$(TARGET_ARCH)" "pdaxrom"
  SUBDIRS += src/desktop/dialogs src/unix/message
endif

ifeq "<$(TARGET_ARCH)>" "$(findstring <$TARGET_ARCH()>, <openzaurus> <opensimpad-0.9.0> <qvfb>)"
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
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS)) $(XML_LIB) $(ARCHIVER_LIB) -ldl
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
	@install -d $(SHARE_ZLIBRARY)/encodings
	@install -m 0644 $(ENCODING_FILES) $(SHARE_ZLIBRARY)/encodings
	@install -d $(SHARE_ZLIBRARY)/resources
	@install -m 0644 $(wildcard data/resources/*.xml) $(SHARE_ZLIBRARY)/resources
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif

package:

clean:
	@for subdir in $(SUBDIRS_ALL); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.* *.a
