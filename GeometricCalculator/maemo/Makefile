ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)
DEBIAN_VERSION = $(VERSION)-1
ARCH	= $(shell dpkg-architecture -qDEB_HOST_ARCH)
PACKAGE_NAME = geometriccalculator-$(UI_TYPE)
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
DEPENDS = `dpkg-shlibdeps -O ../GeometricCalculator | sed -e 's/shlibs:Depends=//'`
MENUDIR = $(DESTDIR)/etc/others-menu
SECTION = user/tools
endif

TMPDIR = tmp
DESTDIR = $(TMPDIR)

all: package
	@mv $(PACKAGE_FILE) ../..

DESKTOP_FILE = GeometricCalculator.desktop
SERVICE_FILE = GeometricCalculator.service
BACKUP_CONF = GeometricCalculator.backup
SHARE_GCALCULATOR = $(DESTDIR)$(SHAREDIR)/GeometricCalculator

package:
	@cd ..; DESTDIR=$(PWD)/$(DESTDIR) $(MAKE) do_install
	@mkdir -p $(DESTDIR)/DEBIAN
	@install -d $(DESTDIR)$(SHAREDIR)/GeometricCalculator
	@install -d $(SHARE_GCALCULATOR)/icons/GeometricCalculator
	@install -m 0644 $(wildcard ../icons/maemo/GeometricCalculator/*) $(SHARE_GCALCULATOR)/icons/GeometricCalculator
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -m 0644 ../icons/maemo/GeometricCalculator.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -d $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -m 0644 $(DESKTOP_FILE) $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -d $(MENUDIR)
	@cd $(MENUDIR); ln -sf ../../../usr/share/applications/hildon/$(DESKTOP_FILE) 0000_$(DESKTOP_FILE)
	@install -d $(DESTDIR)$(DBUSDIR)
	@install -m 0644 $(SERVICE_FILE) $(DESTDIR)$(DBUSDIR)
	@install -d $(DESTDIR)/etc/osso-backup/applications
	@install -m 0644 $(BACKUP_CONF) $(DESTDIR)/etc/osso-backup/applications/GeometricCalculator.conf
	@sed -e "s/@VERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$(PACKAGE_NAME)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		-e "s#@SECTION@#$(SECTION)#" \
		GeometricCalculator.control.in > $(TMPDIR)/DEBIAN/control
	@install GeometricCalculator.postinst $(TMPDIR)/DEBIAN/postinst
	@fakeroot dpkg-deb -b $(TMPDIR) $(PACKAGE_FILE)
	@rm -rf $(TMPDIR)

clean:
	@$(RM) *.deb
