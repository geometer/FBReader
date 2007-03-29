ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)
DEBIAN_REVISION ?= 1
DEBIAN_VERSION = $(VERSION)-$(DEBIAN_REVISION)
ARCH	= $(shell dpkg-architecture -qDEB_HOST_ARCH)
PACKAGE_NAME = libzlibrary-$(UI_TYPE)
PACKAGE_FILE = $(PACKAGE_NAME)_$(DEBIAN_VERSION)_$(ARCH).deb

ifeq "$(UI_TYPE)" "maemo"
PREFIX = /var/lib/install
DEPENDS = maemo
SECTION = libs
else
PREFIX =
DEPENDS = `dpkg-shlibdeps -O ../libzlibrary-$(UI_TYPE).so | sed -e 's/shlibs:Depends=//'`
SECTION = user/libs
endif

TMPDIR = tmp
DESTDIR = $(TMPDIR)

all: package
	@mv $(PACKAGE_FILE) ../..

package:
	@cd ..; DESTDIR=$(PWD)/$(DESTDIR) $(MAKE) do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@sed -e "s/@DEBIANVERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$(PACKAGE_NAME)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		-e "s#@SECTION@#$(SECTION)#" \
		-e "s#@CONFLICTS@#fbreader-$(UI_TYPE) (<< 0.8.0)#" \
		control.in > $(TMPDIR)/DEBIAN/control
	@sed -e "s/@DEBIANVERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@VERSION@#$(VERSION)#" \
		shlibs.in > $(TMPDIR)/DEBIAN/shlibs
	@install -m 0755 postinst $(TMPDIR)/DEBIAN/postinst
	@install -m 0755 postrm $(TMPDIR)/DEBIAN/postrm
	@fakeroot dpkg-deb -b $(TMPDIR) $(PACKAGE_FILE)
	@rm -rf $(TMPDIR)

clean:
	@$(RM) *.deb
