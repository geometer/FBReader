ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

install:
	@install -m 0644 ../data/default/config.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(TARGET_ARCH).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/pics
	@install -d $(DESTDIR)$(INSTALLDIR)/.FBReader
	@install -m 0644 ../data/icons/application/32x24.png $(DESTDIR)$(INSTALLDIR)/pics/FBReader-s.png
	@install -m 0644 ../data/icons/application/64x43.png $(DESTDIR)$(INSTALLDIR)/pics/FBReader-b.png
	@install -m 0644 ../data/icons/application/48x48.png $(DESTDIR)$(INSTALLDIR)/pics/FBReader.png
	@install -m 0644 FBReader.desktop $(DESTDIR)$(INSTALLDIR)/FBReader.desktop
	@install FBReader.sh $(DESTDIR)$(BINDIR)
	@install util/language_detector $(DESTDIR)$(BINDIR)

clean:
