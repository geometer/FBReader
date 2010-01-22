ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(SHAREDIR)/FBReader

install:
	@install -d $(INSTALLDIR)/Contents/Resources/English.lproj
	@install -m 0644 Info.plist $(INSTALLDIR)/Contents
	@install -m 0644 ../data/icons/application/FBReader.icns $(INSTALLDIR)/Contents/Resources
	@$(TOOLS_DIR)/ibtool --errors --warnings --notices --output-format human-readable-text --compile $(INSTALLDIR)/Contents/Resources/English.lproj/MainMenu.nib $(ROOTDIR)/fbreader/macosx/English.lproj/MainMenu.xib
	@install -m 0644 ../data/default/config.macos.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml
