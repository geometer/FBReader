ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

TARGET = FBReader

VERSION = $(shell cat ../VERSION)

TARBALL = fbreader-$(UI_TYPE)-$(VERSION).tgz
DEBIAN_PACKAGE = fbreader-$(UI_TYPE)_$(VERSION)-1_i386.deb

TMPDIR = tmp

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

DEPENDS = `dpkg-shlibdeps -O ../FBReader | sed -e 's/shlibs:Depends=//'`

all: tarball debpackage
	@cp $(TARBALL) $(DEBIAN_PACKAGE) ../..

install:
	@install -d $(DESTDIR)/usr/share/applications
	@install -m 0644 desktop $(DESTDIR)/usr/share/applications/$(TARGET).desktop
	@install -d $(DESTDIR)$(IMAGEDIR)
	@install -m 0644 ../icons/34x28/FBReader.png $(DESTDIR)$(IMAGEDIR)
	@install -d $(DESTDIR)$(IMAGEDIR)/FBReader
	@install -m 0644 $(wildcard ../icons/34x28/FBReader/*) $(DESTDIR)$(IMAGEDIR)/FBReader
	@install -d $(SHARE_FBREADER)/help
	@install -m 0644 ../data/help/HowToStart.brownish.640x480.fb2 $(SHARE_FBREADER)/help/HowToStart.fb2
	@install -d $(SHARE_FBREADER)/default
	@install -m 0644 ../data/default/config.desktop.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml

tarball:
	@cd ..; make DESTDIR=$(PWD)/$(TMPDIR) do_install
	@fakeroot tar czf $(TARBALL) -C $(TMPDIR) .
	@$(RM_QUIET) $(TMPDIR)

debpackage:
	@cd ..; make DESTDIR=$(PWD)/$(TMPDIR) do_install
	@mkdir -p $(TMPDIR)/DEBIAN
	@sed -e "s/@VERSION@/$(VERSION)/" \
		-e "s#@UI_TYPE@#$(UI_TYPE)#" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		debian/control.in > $(TMPDIR)/DEBIAN/control
	@cat $(TMPDIR)/DEBIAN/control
	@fakeroot dpkg -b $(TMPDIR) $(DEBIAN_PACKAGE)
	@$(RM_QUIET) $(TMPDIR)

clean:
	@$(RM) $(TMPDIR) *.tgz *.deb *.rpm
