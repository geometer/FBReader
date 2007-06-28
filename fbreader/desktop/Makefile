ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

TARGET = FBReader

VERSION = $(shell cat ../VERSION)
DEBIAN_REVISION ?= 1
DEBIAN_VERSION = $(VERSION)-$(DEBIAN_REVISION)
ARCH = $(shell dpkg-architecture -qDEB_HOST_ARCH)

TARBALL = fbreader-$(VERSION).tgz
DEBIAN_PACKAGE = fbreader-$(DEBIAN_VERSION)_i386.deb

TMPDIR = tmp

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

DEPENDS = `dpkg-shlibdeps -O ../FBReader | sed -e 's/shlibs:Depends=//'`, libzlui

all: tarball debpackage
	@cp $(TARBALL) $(DEBIAN_PACKAGE) ../..

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

debpackage:
	@cd ..; make DESTDIR=$(CURDIR)/$(TMPDIR) do_install
	@mkdir -p $(TMPDIR)/DEBIAN
	@sed -e "s/@VERSION@/$(VERSION)/" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		debian/control.in > $(TMPDIR)/DEBIAN/control
	@fakeroot dpkg -b $(TMPDIR) $(DEBIAN_PACKAGE)
	@$(RM_QUIET) $(TMPDIR)

clean:
	@$(RM) $(TMPDIR) *.tgz *.deb *.rpm
