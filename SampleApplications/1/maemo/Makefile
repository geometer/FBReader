ROOTDIR = $(PWD)/../../..
include $(ROOTDIR)/makefiles/config.mk

TARGET = SampleApplication

VERSION = $(shell cat ../VERSION)
DEBIAN_VERSION = $(VERSION)-1
ARCH	= $(shell dpkg-architecture -qDEB_HOST_ARCH)
PACKAGE_NAME = sampleapplication-$(UI_TYPE)
PACKAGE_FILE = $(PACKAGE_NAME)_$(DEBIAN_VERSION)_$(ARCH).deb

ifeq "$(UI_TYPE)" "maemo"
PREFIX = /var/lib/install
DBUSDIR = /usr/lib/dbus-1.0/services
DEPENDS = maemo,libzlibrary-maemo
MENUDIR = $(DESTDIR)/etc/others-menu/extra_applications
SECTION = utils
else
PREFIX =
DBUSDIR = /usr/share/dbus-1/services
DEPENDS = `dpkg-shlibdeps -O ../$(TARGET) | sed -e 's/shlibs:Depends=//'`
MENUDIR = $(DESTDIR)/etc/others-menu
SECTION = user/tools
endif

TMPDIR = tmp
DESTDIR = $(TMPDIR)

all: package
	@mv $(PACKAGE_FILE) $(ROOTDIR)

package:
	@cd ..; DESTDIR=$(PWD)/$(DESTDIR) $(MAKE) do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@install -d $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -m 0644 desktop $(DESTDIR)$(SHAREDIR)/applications/hildon/$(TARGET).desktop
	@install -d $(MENUDIR)
	@cd $(MENUDIR); ln -sf ../../../usr/share/applications/hildon/$(TARGET).desktop 0000_$(TARGET).desktop
	@install -d $(DESTDIR)$(DBUSDIR)
	@install -m 0644 service $(DESTDIR)$(DBUSDIR)/$(TARGET).service
	@sed -e "s/@VERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$(PACKAGE_NAME)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		-e "s#@SECTION@#$(SECTION)#" \
		control.in > $(TMPDIR)/DEBIAN/control
	@install postinst $(TMPDIR)/DEBIAN/postinst
	@fakeroot dpkg-deb -b $(TMPDIR) $(PACKAGE_FILE)
	@rm -rf $(TMPDIR)

clean:
	@$(RM) *.deb
