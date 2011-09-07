ROOTDIR = $(CURDIR)/../..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

VERSION = $(shell cat ../VERSION)
SOVERSION = $(shell cat SOVERSION)

ifeq "$(ZLSHARED)" "yes"
  ifeq "$(TARGET_ARCH)" "macosx"
    TARGET = libzltext.$(VERSION).dylib
    TARGET_SONAME = libzltext.$(SOVERSION).dylib
    TARGET_SHORTNAME = libzltext.dylib
  else
    TARGET = libzltext.so.$(VERSION)
    TARGET_SONAME = libzltext.so.$(SOVERSION)
    TARGET_SHORTNAME = libzltext.so
  endif
else
  TARGET = libzltext.a
endif

SUBDIRS = src/model src/area src/view src/style src/styleOptions src/hyphenation

.objects:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			if ! $(LIBMAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
				exit 1; \
			fi; \
		fi; \
	done;

$(TARGET): .objects
	@echo -n 'Creating $@ ...'
	@$(RM_QUIET) $(TARGET)
ifeq "$(ZLSHARED)" "yes"
	@$(LD) $(LDFLAGS) -shared -Wl,-soname,$(TARGET_SONAME) -o $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS)) $(CORE_LIBS) -llinebreak -lfribidi
	#@$(LD) $(LDFLAGS) -dynamiclib -dylinker_install_name $(TARGET_SONAME) -o $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS)) $(CORE_LIBS) $(EXTERNAL_LIBS) -llinebreak -lfribidi
	@ln -sf $(TARGET) $(TARGET_SONAME)
	@ln -sf $(TARGET) $(TARGET_SHORTNAME)
else
	@$(AR) $(TARGET) $(patsubst %, %/*.o, $(SUBDIRS))
endif
	@echo ' OK'

SHARE_ZLIBRARY = $(DESTDIR)$(SHAREDIR)/zlibrary

do_install:
	@install -d $(SHARE_ZLIBRARY)
	@install -m 0644 $(wildcard data/hyphenationPatterns.zip) $(SHARE_ZLIBRARY)
ifeq "$(ZLSHARED)" "yes"
	@install -d $(DESTDIR)$(LIBDIR)
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
	@ln -sf $(TARGET) $(DESTDIR)$(LIBDIR)/$(TARGET_SONAME)
endif

do_install_dev:
	@install -d $(DESTDIR)$(LIBDIR)
ifeq "$(ZLSHARED)" "yes"
	@ln -sf $(TARGET) $(DESTDIR)$(LIBDIR)/$(TARGET_SHORTNAME)
else
	@install $(TARGET) $(DESTDIR)$(LIBDIR)
endif
	@install -d $(DESTDIR)$(INCDIR)/zlibrary/text
	@install -m 0644 $(wildcard include/*.h) $(DESTDIR)$(INCDIR)/zlibrary/text

clean:
	@for subdir in $(SUBDIRS); do \
		if [ -d $$subdir ]; then \
			$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
		fi; \
	done;
	@$(RM) *.so *.so.* *.dylib *.a
