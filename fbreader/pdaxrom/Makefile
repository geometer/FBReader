ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

all:

install:
	@install -m 0644 ../data/default/config.pdaxrom.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.pdaxrom.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.640x480.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -m 0644 ../icons/34x28/FBReader.png $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -m 0644 $(wildcard ../icons/34x28/FBReader/*.png) $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -d $(DESTDIR)/$(INSTALLDIR)/apps/Office/FBReader
	@install -m 0644 data/apps/Office/FBReader/AppRun $(DESTDIR)$(INSTALLDIR)/apps/Office/FBReader
	@install -m 0644 data/apps/Office/FBReader/AppInfo.xml $(DESTDIR)$(INSTALLDIR)/apps/Office/FBReader
	@install -d $(DESTDIR)/$(SHAREDIR)/applications
	@install -m 0644 data/share/applications/FBReader.desktop $(DESTDIR)/$(SHAREDIR)/applications

clean:
