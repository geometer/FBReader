ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

DBUSDIR = /usr/share/dbus-1/services
MENUDIR = /etc/others-menu

DESKTOP_FILE = FBReader.desktop
SERVICE_FILE = FBReader.service
BACKUP_CONF = FBReader.backup

install:
	@install -m 0644 ../data/default/config.maemo.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.maemo.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.maemo.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/16x16/apps
	@install -m 0644 ../data/icons/application/16x16.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/16x16/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -m 0644 ../data/icons/application/26x26.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/48x48/apps
	@install -m 0644 ../data/icons/application/48x48.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/48x48/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/64x64/apps
	@install -m 0644 ../data/icons/application/64x64.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/64x64/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -m 0644 $(DESKTOP_FILE) $(DESTDIR)$(SHAREDIR)/applications/hildon
	@install -d $(DESTDIR)$(MENUDIR)
	@cd $(DESTDIR)$(MENUDIR); ln -sf ../../../usr/share/applications/hildon/$(DESKTOP_FILE) 0000_$(DESKTOP_FILE)
	@install -d $(DESTDIR)$(DBUSDIR)
	@install -m 0644 $(SERVICE_FILE) $(DESTDIR)$(DBUSDIR)
	@install -d $(DESTDIR)/etc/osso-backup/applications
	@install -m 0644 $(BACKUP_CONF) $(DESTDIR)/etc/osso-backup/applications/FBReader.conf

clean:
