ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

SHARE_FBREADER = $(DESTDIR)/share/FBReader

install:
	@install -m 0644 ../data/default/keymap.win32.xml $(SHARE_FBREADER)/default/keymap.xml
	@install -m 0644 ../data/default/styles.win32.xml $(SHARE_FBREADER)/default/styles.xml

clean:
