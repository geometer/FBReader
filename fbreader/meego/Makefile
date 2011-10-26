ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

DBUSDIR = /usr/share/dbus-1/services
MENUDIR = /etc/others-menu

DESKTOP_FILE = FBReader.desktop
SERVICE_FILE = FBReader.service
BACKUP_CONF = FBReader.backup

DESKTOP_DIR = ""
DESKTOP_FILE = FBReader.desktop
KEYMAP_FILE = "keymap.meego.xml"

install:
	@install -m 0644 ../data/default/$(KEYMAP_FILE) $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.maemo.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/16x16/apps
	@install -m 0644 ../data/icons/application/16x16.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/16x16/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps
	@install -m 0644 ../data/icons/application/26x26.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/26x26/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/48x48/apps
	@install -m 0644 ../data/icons/application/48x48.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/48x48/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/64x64/apps
	@install -m 0644 ../data/icons/application/64x64.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/64x64/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/icons/hicolor/80x80/apps
	@install -m 0644 ../data/icons/application/meego_80x80.png $(DESTDIR)$(SHAREDIR)/icons/hicolor/80x80/apps/FBReader.png
	@install -d $(DESTDIR)$(SHAREDIR)/applications/$(DESKTOP_DIR)
	@install -m 0644 $(DESKTOP_FILE) $(DESTDIR)$(SHAREDIR)/applications/$(DESKTOP_DIR)/FBReader.desktop

clean:
