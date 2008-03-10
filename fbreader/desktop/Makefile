ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

TARGET = FBReader

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

install:
	@install -d $(DESTDIR)/usr/share/applications
	@install -m 0644 desktop $(DESTDIR)/usr/share/applications/$(TARGET).desktop
	@install -d $(DESTDIR)$(IMAGEDIR)
	@install -m 0644 ../data/icons/application/$(TARGET_ARCH).png $(DESTDIR)$(IMAGEDIR)/FBReader.png
	@install -m 0644 ../data/default/config.desktop.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml

clean:
