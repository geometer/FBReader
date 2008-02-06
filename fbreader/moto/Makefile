ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat $(ROOTDIR)/zlibrary/VERSION)
SOVERSION = $(shell cat $(ROOTDIR)/zlibrary/SOVERSION)

PICS_TO = pics

install:
	@install -m 0644 ../data/default/config.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -d $(DESTDIR)$(INSTALLDIR)/.FBReader
	@install -m 0744 $(ARM_DEV_DIR)/arm-linux/lib/libz.so.1 $(DESTDIR)$(LIBDIR)/libz.so.1
	@install -m 0744 $(ARM_DEV_DIR)/arm-linux/lib/libexpat.so.1 $(DESTDIR)$(LIBDIR)/libexpat.so.1
	@install -m 0744 $(ARM_DEV_DIR)/arm-linux/lib/libbz2.so.1.0 $(DESTDIR)$(LIBDIR)/libbz2.so.1.0
	@install -m 0744 $(DESTDIR)$(LIBDIR)/libzlcore.so.$(VERSION) $(DESTDIR)$(LIBDIR)/libzlcore.so.$(SOVERSION)
	@install -m 0744 $(DESTDIR)$(LIBDIR)/libzltext.so.$(VERSION) $(DESTDIR)$(LIBDIR)/libzltext.so.$(SOVERSION)
	@rm -f $(DESTDIR)$(LIBDIR)/libzlcore.so.$(VERSION)
	@rm -f $(DESTDIR)$(LIBDIR)/libzltext.so.$(VERSION)
	@install -m 0644 ../icons/application/$(TARGET_ARCH)-s.png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader-s.png
	@install -m 0644 ../icons/application/$(TARGET_ARCH)-b.png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader-b.png
	@install -m 0644 FBReader.desktop $(DESTDIR)$(INSTALLDIR)/FBReader.desktop
	@install -m 0744 FBReader.sh $(DESTDIR)$(BINDIR)/FBReader.sh

clean:
