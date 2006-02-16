ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)
DEBIAN_PACKAGE = fbreader-$(UI_TYPE)_$(VERSION)-1_arm.deb

BINDIR=/usr/bin
TMPDIR=tmp

all: package
	@cp $(DEBIAN_PACKAGE) ../..

#install: FBReader.desktop net.mawhrin.FBReader.services
install: FBReader.desktop
	@install -d $(DESTDIR)/usr/bin
	@install -s ../FBReader $(DESTDIR)/usr/bin
	@install -d $(DESTDIR)/usr/share/FBReader/
	@install -d $(DESTDIR)/usr/share/FBReader/formats/html
	@install -m 0644 ../share/FBReader/formats/html/html.ent $(DESTDIR)/usr/share/FBReader/formats/html
	@install -d $(DESTDIR)/usr/share/FBReader/icons/FBReader
	@install -m 0644 $(wildcard ../icons/blueset/FBReader/*) $(DESTDIR)/usr/share/FBReader/icons/FBReader
	@install -d $(DESTDIR)/usr/share/FBReader/encodings
	@install -m 0644 $(filter-out %/Big5,$(wildcard ../share/FBReader/encodings/*)) $(DESTDIR)/usr/share/FBReader/encodings
	@install -m 0644 $(wildcard ../share/FBReader/*.zip) $(DESTDIR)/usr/share/FBReader
	@install -d $(DESTDIR)/usr/share/FBReader/help
	@install -m 0644 ../data/help/HowToStart.blueish.fb2 $(DESTDIR)/usr/share/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)/usr/share/FBReader/default
	@install -m 0644 ../data/default/keymap.maemo.xml $(DESTDIR)/usr/share/FBReader/default/keymap.xml
	@install -d $(DESTDIR)/usr/share/icons/hicolor/26x26/apps
	@install -m 0644 ../icons/blueset/FBReader.png $(DESTDIR)/usr/share/icons/hicolor/26x26/apps
	@install -d $(DESTDIR)/usr/share/applications/hildon
	@install -m 0644 FBReader.desktop $(DESTDIR)/usr/share/applications/hildon
	@install -d $(DESTDIR)/etc/others-menu/extra_applications
	@cd $(DESTDIR)/etc/others-menu/extra_applications; ln -sf ../../../usr/share/applications/hildon/FBReader.desktop 0000_FBReader.desktop

FBReader.desktop: FBReader.desktop.in
	sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

net.mawhrin.FBReader.services: net.mawhrin.FBReader.services.in
	sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

package:
	@mkdir $(TMPDIR)
	make DESTDIR=$(PWD)/$(TMPDIR) install
	@mkdir -p $(TMPDIR)/DEBIAN
	@sed "s/VERSION/$(VERSION)/" FBReader.control.in > $(TMPDIR)/DEBIAN/control
	@fakeroot dpkg-deb -b $(TMPDIR) $(DEBIAN_PACKAGE)
	@rm -rf $(TMPDIR)

clean:
	$(RM) FBReader.desktop net.mawhrin.FBReader.services *.deb
