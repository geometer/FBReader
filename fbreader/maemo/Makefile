ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

VERSION = $(shell cat ../VERSION)
DEBIAN_REVISION ?= 1
DEBIAN_VERSION = $(VERSION)-$(DEBIAN_REVISION)
ARCH	= $(shell dpkg-architecture -qDEB_HOST_ARCH)
PACKAGE_NAME = fbreader-$(UI_TYPE)
PACKAGE_FILE = $(PACKAGE_NAME)_$(DEBIAN_VERSION)_$(ARCH).deb

ifeq "$(UI_TYPE)" "maemo"
PREFIX = /var/lib/install
DBUSDIR = /usr/lib/dbus-1.0/services
DEPENDS = maemo,libenca0,libzlibrary-maemo
MENUDIR = $(DESTDIR)/etc/others-menu/extra_applications
SECTION = utils
else
PREFIX =
DBUSDIR = /usr/share/dbus-1/services
DEPENDS = `dpkg-shlibdeps -O ../FBReader | sed -e 's/shlibs:Depends=//'`
MENUDIR = $(DESTDIR)/etc/others-menu
SECTION = user/tools
endif

TMPDIR = tmp

all: package
	@mv $(PACKAGE_FILE) ../..

DESKTOP_FILE = FBReader.desktop
SERVICE_FILE = FBReader.service
BACKUP_CONF = FBReader.backup
SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

install: $(DESKTOP_FILE) $(SERVICE_FILE)
	@install -d $(SHARE_FBREADER)/icons
	@install -m 0644 $(wildcard ../icons/blueset/FBReader/*.png) $(SHARE_FBREADER)/icons
	@install -m 0644 ../data/default/config.maemo.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.maemo.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.maemo.xml $(SHARE_FBREADER)/default/styles.xml
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -m 0644 ../icons/blueset/FBReader.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -d $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -m 0644 $(DESKTOP_FILE) $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -d $(MENUDIR)
	@cd $(MENUDIR); ln -sf ../../../usr/share/applications/hildon/$(DESKTOP_FILE) 0000_$(DESKTOP_FILE)
	@install -d $(DESTDIR)$(DBUSDIR)
	@install -m 0644 $(SERVICE_FILE) $(DESTDIR)$(DBUSDIR)
	@install -d $(DESTDIR)/etc/osso-backup/applications
	@install -m 0644 $(BACKUP_CONF) $(DESTDIR)/etc/osso-backup/applications/FBReader.conf

$(DESKTOP_FILE): $(DESKTOP_FILE).in
	@sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

$(SERVICE_FILE): $(SERVICE_FILE).in
	@sed -e "s#@BINDIR@#$(BINDIR)#" < $< > $@

package:
	@mkdir -p $(TMPDIR)
	@cd ..; make DESTDIR=$(CURDIR)/$(TMPDIR) do_install
	@mkdir -p $(TMPDIR)/DEBIAN
	@sed -e "s/@VERSION@/$(DEBIAN_VERSION)/" \
		-e "s#@PACKAGE@#$(PACKAGE_NAME)#" \
		-e "s#@ARCH@#$(ARCH)#" \
		-e "s#@SIZE@#`du -s -k $(TMPDIR) | cut -f1`#" \
		-e "s#@DEPENDS@#$(DEPENDS)#" \
		-e "s#@SECTION@#$(SECTION)#" \
		FBReader.control.in > $(TMPDIR)/DEBIAN/control
ifeq "$(UI_TYPE)" "maemo2"
	@install FBReader.postinst $(TMPDIR)/DEBIAN/postinst
endif
	@fakeroot dpkg-deb -b $(TMPDIR) $(PACKAGE_FILE)
	@rm -rf $(TMPDIR)

clean:
	@$(RM) $(DESKTOP_FILE) $(SERVICE_FILE) *.deb
