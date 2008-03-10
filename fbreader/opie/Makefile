ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

install:
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -d $(DESTDIR)$(INSTALLDIR)/pics/fbreader
	@install -m 0644 fbreader.desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 ../data/icons/application/$(TARGET_ARCH).png $(DESTDIR)$(INSTALLDIR)/pics/fbreader/FBReader.png
ifeq "$(TARGET_ARCH)" "opensimpad-0.9.0"
	@install -m 0644 ../data/default/config.opie.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.opie-opensimpad.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.640x480.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
endif

clean:
