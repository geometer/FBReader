ROOTDIR = $(CURDIR)/../..
include $(ROOTDIR)/makefiles/config.mk

DESTDIR = data

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

DEBDIR = data/DEBIAN

version = $(shell cat ../VERSION)
package = fbreader-$(UI_TYPE)_$(version)_$(TARGET_ARCH)_$(RESOLUTION)_arm.ipk

all:
	@RESOLUTION=240x320 $(MAKE) ipk
	@RESOLUTION=640x480 $(MAKE) ipk

ipk:
	@make .builddir
	@make .buildpackage
	@make .cleandir

install:
	@install -d $(DESTDIR)$(PICSDIR)
	@install -m 0644 ../icons/$(ICONSIZE)/FBReader.png $(DESTDIR)$(PICSDIR)
	@install -m 0644 $(wildcard ../icons/$(ICONSIZE)/FBReader/*.png) $(DESTDIR)$(PICSDIR)
	@install -m 0644 ../data/default/config.openzaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(STYLE).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(APPDIR)
	@install -m 0644 $(UI_TYPE)/fbreader.desktop $(DESTDIR)$(APPDIR)

clean: .cleandir
	@rm -f *.ipk

.builddir: .cleandir
	@mkdir -p $(DEBDIR)
	@sed "s/VERSION/$(version)/" $(UI_TYPE)/control.in > $(DEBDIR)/control
	@cd ../../zlibrary/core; make DESTDIR=$(CURDIR)/$(DESTDIR) do_install
	@cd ../../zlibrary/text; make DESTDIR=$(CURDIR)/$(DESTDIR) do_install
	@cd ../../zlibrary/ui; make DESTDIR=$(CURDIR)/$(DESTDIR) do_install
	@cd ..; make RESOLUTION=$(RESOLUTION) DESTDIR=$(CURDIR)/$(DESTDIR) do_install

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../..

.cleandir:
	@rm -rf data
