ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/platforms.mk

ifeq "$(UI_TYPE)" "opie"
	BASEDIR = data/opt/QtPalmtop
	PICSDIR = $(BASEDIR)/pics/fbreader
	APPDIR = $(BASEDIR)/apps/Applications
	STYLE = $(RESOLUTION)
else # UI_TYPE == gpe
	BASEDIR = data/usr
	PICSDIR = $(BASEDIR)/share/pixmaps/fbreader
	APPDIR = $(BASEDIR)/share/applications
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

clean: .cleandir
	@rm -f *.ipk

.builddir: .cleandir
	@mkdir -p $(DEBDIR)
	@sed "s/VERSION/$(version)/" $(UI_TYPE)/control.in > $(DEBDIR)/control
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/share/zlibrary
	@cp ../../zlibrary/data/keynames.openzaurus-$(UI_TYPE).xml $(BASEDIR)/share/zlibrary/keynames.xml
	@cp -r ../../zlibrary/data/encodings $(BASEDIR)/share/zlibrary
	@cp ../FBReader $(BASEDIR)/bin
	@mkdir -p $(PICSDIR)
	@cp ../icons/$(ICONSIZE)/FBReader.png $(PICSDIR)
	@cp ../icons/$(ICONSIZE)/FBReader/*.png $(PICSDIR)
	@cp -r ../share  $(BASEDIR)
	@rm -rf `find $(BASEDIR)/share -name ".svn"`
	@mkdir $(BASEDIR)/share/FBReader/help
	@cp ../data/help/HowToStart.brownish.$(RESOLUTION).fb2 $(BASEDIR)/share/FBReader/help/HowToStart.fb2
	@mkdir $(BASEDIR)/share/FBReader/default
	@cp ../data/default/config.openzaurus.xml $(BASEDIR)/share/FBReader/default/config.xml
	@cp ../data/default/keymap.zaurus.xml $(BASEDIR)/share/FBReader/default/keymap.xml
	@cp ../data/default/styles.$(STYLE).xml $(BASEDIR)/share/FBReader/default/styles.xml
	@mkdir -p $(APPDIR)
	@cp $(UI_TYPE)/fbreader.desktop $(APPDIR)

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../..

.cleandir:
	@rm -rf data
