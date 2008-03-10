ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

install:
	@install -m 0644 ../data/default/config.pma400.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.pma400.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.240x320.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/pics
	@install -m 0644 ../data/icons/application/$(TARGET_ARCH).png $(DESTDIR)$(INSTALLDIR)/pics/FBReader.png
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 FBReader.desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/FBReader.desktop

clean:
