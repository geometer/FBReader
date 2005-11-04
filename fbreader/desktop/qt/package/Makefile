DESTDIR = $(shell pwd)/dest
VERSION = $(shell cat ../../VERSION)

TARBALL = fbreader-qt-$(VERSION).tgz
DEBIAN_PACKAGE = fbreader-qt_$(VERSION)-1_i386.deb

all: .tarball .deb-package .clean
	@cp $(TARBALL) $(DEBIAN_PACKAGE) ../../..

.install:
	@( cd ../..; make DESTDIR=$(DESTDIR) .really-install );

.tarball: .install
	@fakeroot tar czf $(TARBALL) -C $(DESTDIR) .

.deb-package: .install
	@mkdir -p $(DESTDIR)/DEBIAN
	@sed "s/VERSION/$(VERSION)/" control.in > $(DESTDIR)/DEBIAN/control
	@fakeroot dpkg -b $(DESTDIR) $(DEBIAN_PACKAGE)

.clean:
	@rm -rf $(DESTDIR)

clean: .clean
	@rm -rf *.tgz *.deb
