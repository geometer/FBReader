ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

DESTDIR = $(PWD)/
SHARE_FBREADER = $(DESTDIR)share/FBReader

all:
	@cd ../../zlibrary; make DESTDIR=$(DESTDIR) SHAREDIR=share do_install
	@cd ..; make DESTDIR=$(DESTDIR) SHAREDIR=share do_install
	@install -d $(DESTDIR)/share/icons
	@install -m 0644 $(wildcard ../icons/win32/FBReader/*.ico) $(DESTDIR)/share/icons
	@install -d $(SHARE_FBREADER)/help
	@install -m 0644 ../data/help/HowToStart.brownish.640x480.fb2 $(SHARE_FBREADER)/help/HowToStart.fb2
	@install -d $(SHARE_FBREADER)/default
	@install -m 0644 ../data/external.win32.xml $(SHARE_FBREADER)/external.xml
	@install -m 0644 ../data/default/keymap.desktop.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.win32.xml $(SHARE_FBREADER)/default/styles.xml
	@makensis FBReader.nsi
	@rm -rf $(DESTDIR)/bin
	@rm -rf $(DESTDIR)/share
	@cp *.exe $(DEBIAN_PACKAGE) ../..

install:

clean:
	@rm -rf $(DESTDIR)/bin
	@rm -rf $(DESTDIR)/share
	@rm -rf *.exe
