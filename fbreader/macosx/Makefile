ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

install:
	@strip $(DESTDIR)$(BINDIR)/*
	@install -m 0644 Info.plist $(DESTDIR)$(INSTALLDIR)/Contents
	@install -m 0644 ../data/icons/application/FBReader.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@install -m 0644 ../data/icons/application/FBReaderDocument.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@install -m 0644 ../data/default/config.macosx.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.macosx.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml
