ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

ifeq "$(UI_TYPE)" "maemo"
DBUSDIR = /usr/lib/dbus-1.0/services
MENUDIR = $(DESTDIR)/etc/others-menu/extra_applications
else
DBUSDIR = /usr/share/dbus-1/services
MENUDIR = $(DESTDIR)/etc/others-menu
endif

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

clean:
	@$(RM) $(DESKTOP_FILE) $(SERVICE_FILE)
