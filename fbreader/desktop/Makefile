ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)

TARBALL = fbreader-$(UI_TYPE)-$(VERSION).tgz
DEBIAN_PACKAGE = fbreader-$(UI_TYPE)_$(VERSION)-1_i386.deb

TMPDIR = tmp

SHARE_FBREADER = $(DESTDIR)$(INSTALLDIR)/share/FBReader
SHARE_ZLIBRARY = $(DESTDIR)$(INSTALLDIR)/share/zlibrary

all: tarball debpackage
	@cp $(TARBALL) $(DEBIAN_PACKAGE) ../..

install:
	@install -d $(SHARE_ZLIBRARY)
	@install -m 0644 ../../zlibrary/data/keynames.desktop-$(UI_TYPE).xml $(SHARE_ZLIBRARY)/keynames.xml
	@install -d $(SHARE_ZLIBRARY)/encodings
	@install -m 0644 $(wildcard ../../zlibrary/data/encodings/*) $(SHARE_ZLIBRARY)/encodings
	@install -d $(DESTDIR)$(INSTALLDIR)/bin
	@install ../FBReader $(DESTDIR)$(INSTALLDIR)/bin
	@install -d $(SHARE_FBREADER)
	@install -d $(SHARE_FBREADER)/formats/html
	@install -m 0644 ../share/FBReader/formats/html/html.ent $(SHARE_FBREADER)/formats/html
	@install -d $(SHARE_FBREADER)/formats/xhtml
	@install -m 0644 $(wildcard ../share/FBReader/formats/xhtml/*.ent) $(SHARE_FBREADER)/formats/xhtml
	@install -d $(SHARE_FBREADER)/icons/FBReader
	@install -m 0644 ../icons/34x28/FBReader.png $(SHARE_FBREADER)/icons
	@install -m 0644 $(wildcard ../icons/34x28/FBReader/*) $(SHARE_FBREADER)/icons/FBReader
	@install -m 0644 $(wildcard ../share/FBReader/*.zip) $(SHARE_FBREADER)
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.640x480.fb2 $(SHARE_FBREADER)/help/HowToStart.fb2
	@install -d $(SHARE_FBREADER)/default
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml

tarball:
	@make DESTDIR=$(TMPDIR) install
	@fakeroot tar czf $(TARBALL) -C $(TMPDIR) .
	@$(RM_QUIET) $(TMPDIR)

debpackage:
	@make DESTDIR=$(TMPDIR) install
	@mkdir -p $(TMPDIR)/DEBIAN
	@sed "s/VERSION/$(VERSION)/" debian/control-$(UI_TYPE).in > $(TMPDIR)/DEBIAN/control
	@fakeroot dpkg -b $(TMPDIR) $(DEBIAN_PACKAGE)
	@$(RM_QUIET) $(TMPDIR)

clean:
	@$(RM) $(TMPDIR) *.tgz *.deb *.rpm
