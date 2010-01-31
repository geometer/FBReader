ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader

install:
	@strip $(DESTDIR)$(INSTALLDIR)/Contents/MacOS/*
	@install -d $(DESTDIR)$(INSTALLDIR)/Contents/Resources/English.lproj
	@install -m 0644 Info.plist $(DESTDIR)$(INSTALLDIR)/Contents
	@install -m 0644 ../data/icons/application/FBReader.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@install -m 0644 ../data/icons/application/FBReaderDocument.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@$(TOOLSPATH)/ibtool --errors --warnings --notices --output-format human-readable-text --compile $(DESTDIR)$(INSTALLDIR)/Contents/Resources/English.lproj/MainMenu.nib $(ROOTDIR)/fbreader/macosx/English.lproj/MainMenu.xib
	@install -m 0644 ../data/default/config.macosx.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.macosx.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml
