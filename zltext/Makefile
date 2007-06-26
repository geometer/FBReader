ROOTDIR = $(CURDIR)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat VERSION)

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  TARGET = libzltext-$(UI_TYPE).so.$(VERSION)
  TARGET_SHORTNAME = libzltext-$(UI_TYPE).so
else
  TARGET = libzltext-$(UI_TYPE).a
endif

SUBDIRS = src/model src/view src/hyphenation

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
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET) -o $(TARGET) -lc $(patsubst %, %/*.o, $(SUBDIRS)) $(UILIBS) $(XML_LIB) $(ARCHIVER_LIB)
	@ln -sf $(TARGET) $(TARGET_SHORTNAME)
else
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS))
endif
	@echo " OK"

SHARE_ZLIBRARY = $(DESTDIR)$(SHAREDIR)/zlibrary

do_install:

do_install1:
	@install -d $(SHARE_ZLIBRARY)
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif

package:

clean:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.* *.a
