ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)
DEBIAN_REVISION ?= 1
DEBIAN_VERSION = $(VERSION)-$(DEBIAN_REVISION)
ARCH = $(shell dpkg-architecture -qDEB_HOST_ARCH)

CORE_DEPENDS = `dpkg-shlibdeps -O ../core/libzlcore.so | sed -e 's/shlibs:Depends=//'`
TEXT_DEPENDS = `dpkg-shlibdeps -O ../text/libzltext.so | sed -e 's/shlibs:Depends=//'`
UI_DEPENDS = `dpkg-shlibdeps -O ../ui/zlui-$(UI_TYPE).so | sed -e 's/shlibs:Depends=//'`

TMPDIR = tmp
DESTDIR = $(TMPDIR)

all: packages
	@mv *.deb ../..

packages: libzlcore libzltext libzlui

libzlcore:
	@DESTDIR=$(CURDIR)/$(DESTDIR) $(MAKE) -C ../core do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@sed -e "s/@DEBIANVERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$@#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(CORE_DEPENDS)#" \
		control-$@.in > $(TMPDIR)/DEBIAN/control
	@echo "$@ $(VERSION) $@ (= $(DEBIAN_VERSION))" > $(TMPDIR)/DEBIAN/shlibs
	@install -m 0755 postinst $(TMPDIR)/DEBIAN/postinst
	@install -m 0755 postrm $(TMPDIR)/DEBIAN/postrm
	@fakeroot dpkg-deb -b $(TMPDIR) $@_$(DEBIAN_VERSION)_$(ARCH).deb
	@rm -rf $(TMPDIR)

libzltext:
	@DESTDIR=$(CURDIR)/$(DESTDIR) $(MAKE) -C ../text do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@sed -e "s/@DEBIANVERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$@#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(TEXT_DEPENDS)#" \
		control-$@.in > $(TMPDIR)/DEBIAN/control
	@echo "$@ $(VERSION) $@ (= $(DEBIAN_VERSION))" > $(TMPDIR)/DEBIAN/shlibs
	@install -m 0755 postinst $(TMPDIR)/DEBIAN/postinst
	@install -m 0755 postrm $(TMPDIR)/DEBIAN/postrm
	@fakeroot dpkg-deb -b $(TMPDIR) $@_$(DEBIAN_VERSION)_$(ARCH).deb
	@rm -rf $(TMPDIR)

libzlui:
	@DESTDIR=$(CURDIR)/$(DESTDIR) $(MAKE) -C ../ui do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@sed -e "s/@DEBIANVERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$@-$(UI_TYPE)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(UI_DEPENDS)#" \
		-e "s#@UI@#$(UI_TYPE)#" \
		control-$@.in > $(TMPDIR)/DEBIAN/control
	@echo "$@ $(VERSION) $@ (= $(DEBIAN_VERSION))" > $(TMPDIR)/DEBIAN/shlibs
	@fakeroot dpkg-deb -b $(TMPDIR) $@-$(UI_TYPE)_$(DEBIAN_VERSION)_$(ARCH).deb
	@rm -rf $(TMPDIR)

clean:
	@$(RM) *.deb
