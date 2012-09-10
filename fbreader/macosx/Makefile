ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(DESTDIR)$(SHAREDIR)/FBReader
FRAMEWORKSDIR = $(DESTDIR)$(INSTALLDIR)/Contents/Frameworks
PLUGINSDIR = $(DESTDIR)$(INSTALLDIR)/Contents/PlugIns
BINARY = $(DESTDIR)$(BINDIR)/FBReader

install:
	@rm -rf $(FRAMEWORKSDIR) $(PLUGINSDIR) $(SHAREDIR)/qt.conf
	@install -d $(FRAMEWORKSDIR)
	@install -d $(PLUGINSDIR)
	@for name in QtCore QtGui QtNetwork; do \
		fw=$$name.framework; \
		fw_full=$$fw/Versions/4/$$name; \
		fw_id=@executable_path/../Frameworks/$$fw_full; \
		cp -Rf $(QTBASEDIR)/lib/$$fw $(FRAMEWORKSDIR)/$$fw; \
		rm -rf $(FRAMEWORKSDIR)/$$fw/Versions/4/$${name}_debug; \
		rm -rf $(FRAMEWORKSDIR)/$$fw/Versions/4/Headers; \
		install_name_tool -id $$fw_id $(FRAMEWORKSDIR)/$$fw_full; \
		install_name_tool -change $(QTBASEDIR)/lib/$$fw_full $$fw_id $(BINARY); \
	done
	@install_name_tool -change $(QTBASEDIR)/lib/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$(FRAMEWORKSDIR)/QtGui.framework/Versions/4/QtGui
	@install_name_tool -change $(QTBASEDIR)/lib/QtCore.framework/Versions/4/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
		$(FRAMEWORKSDIR)/QtNetwork.framework/Versions/4/QtNetwork
	@cp -R $(QTBASEDIR)/plugins/imageformats $(PLUGINSDIR)
	@rm $(PLUGINSDIR)/imageformats/*_debug.dylib
	@rm $(PLUGINSDIR)/imageformats/libqsvg.dylib
	@for plugin in $(PLUGINSDIR)/imageformats/*.dylib; do \
		install_name_tool -change $(QTBASEDIR)/lib/QtGui.framework/Versions/4/QtGui \
			@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui $$plugin; \
		install_name_tool -change $(QTBASEDIR)/lib/QtCore.framework/Versions/4/QtCore \
			@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $$plugin; \
	done
	@strip $(BINARY)
	@install -m 0644 Info.plist $(DESTDIR)$(INSTALLDIR)/Contents
	@install -m 0644 ../data/icons/application/FBReader.icns $(DESTDIR)$(INSTALLDIR)/Contents/Resources
	@install -m 0644 ../data/default/config.macosx.xml $(SHARE_FBREADER)/default/config.xml
	@install -m 0644 ../data/default/keymap.macosx.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.desktop.xml $(SHARE_FBREADER)/default/styles.xml
	@$(QTBASEDIR)/bin/macdeployqt $(DESTDIR)$(INSTALLDIR) -dmg
