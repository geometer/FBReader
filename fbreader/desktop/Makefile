ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)

TARBALL = fbreader-$(UI_TYPE)-$(VERSION).tgz
DEBIAN_PACKAGE = fbreader-$(UI_TYPE)_$(VERSION)-1_i386.deb

TMPDIR = tmp

all: tarball debpackage
	@cp $(TARBALL) $(DEBIAN_PACKAGE) ../..

install:
	@install -d $(DESTDIR)$(INSTALLDIR)/bin
	@install ../FBReader $(DESTDIR)$(INSTALLDIR)/bin
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	@install -m 0644 ../share/FBReader/formats/html/html.ent $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/xhtml
	@install -m 0644 $(wildcard ../share/FBReader/formats/xhtml/*.ent) $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/xhtml
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	@install -m 0644 ../icons/34x28/FBReader.png $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons
	@install -m 0644 $(wildcard ../icons/34x28/FBReader/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	@install -m 0644 $(wildcard ../share/FBReader/encodings/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	@install -m 0644 $(wildcard ../share/FBReader/*.zip) $(DESTDIR)$(INSTALLDIR)/share/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.640x480.fb2 $(DESTDIR)$(INSTALLDIR)/share/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/default
	@install -m 0644 ../data/default/keymap.desktop.xml $(DESTDIR)$(INSTALLDIR)/share/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(DESTDIR)$(INSTALLDIR)/share/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/share/zlibrary
	@install -m 0644 ../../zlibrary/data/keynames.desktop.xml $(DESTDIR)$(INSTALLDIR)/share/zlibrary/keynames.xml

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
