ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

TARGET = FBReader

VERSION = $(shell cat ../VERSION)

TARBALL = fbreader-$(VERSION).tgz

TMPDIR = tmp

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

DEPENDS = `dpkg-shlibdeps -O ../FBReader | sed -e 's/shlibs:Depends=//'`, libzlui

all: tarball
	@cp $(TARBALL) ../..

install:
	@install -d $(DESTDIR)/usr/share/applications
	@install -m 0644 desktop $(DESTDIR)/usr/share/applications/$(TARGET).desktop
	@install -d $(DESTDIR)$(IMAGEDIR)
	@install -m 0644 ../icons/tango-green-blue/FBReader.png $(DESTDIR)$(IMAGEDIR)
	@install -d $(DESTDIR)$(IMAGEDIR)/FBReader
	@install -m 0644 $(wildcard ../icons/tango-green-blue/FBReader/*) $(DESTDIR)$(IMAGEDIR)/FBReader
	@install -m 0644 ../data/default/config.desktop.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml

tarball:
	@cd ../../zlibrary; make DESTDIR=$(CURDIR)/$(TMPDIR) do_install
	@cd ..; make DESTDIR=$(CURDIR)/$(TMPDIR) do_install
	@fakeroot tar czf $(TARBALL) -C $(TMPDIR) .
	@$(RM_QUIET) $(TMPDIR)

clean:
	@$(RM) $(TMPDIR) *.tgz *.rpm
