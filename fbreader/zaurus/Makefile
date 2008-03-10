ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

ifeq "$(RESOLUTION)" "240x320"
  PICS_TO = pics
else # RESOLUTION == 640x480
  PICS_TO = pics144
endif

install:
	@install -m 0644 ../data/default/config.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(RESOLUTION).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -m 0644 ../data/icons/application/$(TARGET_ARCH)_$(RESOLUTION).png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader.png
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 FBReader.$(RESOLUTION).desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/FBReader.desktop

clean:
