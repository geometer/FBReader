ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader
BINARY = $(DESTDIR)$(BINDIR)/FBReader

install:
	@strip $(BINARY)
	@install -m 0644 Info.plist $(DESTDIR)$(INSTALLDIR)/Contents
	@install -m 0644 ../data/icons/application/FBReader.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@install -m 0644 ../data/default/config.macosx.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.macosx.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml
	@macdeployqt $(DESTDIR)$(INSTALLDIR)
	@for fw in QtDeclarative QtScript QtSql QtSvg QtXmlPatterns; do \
		rm -rf $(DESTDIR)$(INSTALLDIR)/Contents/Frameworks/$$fw.framework; \
	done
	@for plugin in accessible codecs bearer graphicssystems qmltooling; do \
		rm -rf $(DESTDIR)$(INSTALLDIR)/Contents/PlugIns/$$plugin; \
	done
