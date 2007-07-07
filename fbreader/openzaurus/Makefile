ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

ifeq "$(UI_TYPE)" "opie"
	PICSDIR = $(INSTALLDIR)/pics/fbreader
	APPDIR = $(INSTALLDIR)/apps/Applications
	STYLE = $(RESOLUTION)
else # UI_TYPE == gpe
	PICSDIR = $(SHAREDIR)/pixmaps/fbreader
	APPDIR = $(SHAREDIR)/applications
	STYLE = gpe$(RESOLUTION)
endif

ifeq "$(RESOLUTION)" "240x320"
  ICONSIZE = 18x15
else
  ICONSIZE = 34x28
endif

install:
	@install -d $(DESTDIR)$(PICSDIR)
	@install -m 0644 ../icons/$(ICONSIZE)/FBReader.png $(DESTDIR)$(PICSDIR)
	@install -m 0644 $(wildcard ../icons/$(ICONSIZE)/FBReader/*.png) $(DESTDIR)$(PICSDIR)
	@install -m 0644 ../data/default/config.openzaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(STYLE).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(APPDIR)
	@install -m 0644 $(UI_TYPE)/fbreader.desktop $(DESTDIR)$(APPDIR)

clean:
