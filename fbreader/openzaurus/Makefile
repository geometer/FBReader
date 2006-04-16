ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/platforms.mk

ifeq "$(UI_TYPE)" "opie"
	BASEDIR = data/opt/QtPalmtop
	PICSDIR = $(BASEDIR)/pics/fbreader
	APPDIR = $(BASEDIR)/apps/Applications
	STYLE = 640x480
else # UI_TYPE == gpe
	BASEDIR = data/usr
	PICSDIR = $(BASEDIR)/share/pixmaps/fbreader
	APPDIR = $(BASEDIR)/share/applications
	STYLE = maemo
endif

DEBDIR = data/DEBIAN

version = $(shell cat ../VERSION)
package = fbreader-$(UI_TYPE)_$(version)_$(TARGET_ARCH)_arm.ipk

all:
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
	@cp ../FBReader $(BASEDIR)/bin
	@mkdir -p $(PICSDIR)
	@cp ../icons/22x18/FBReader.png $(PICSDIR)
	@cp ../icons/34x28/FBReader/*.png $(PICSDIR)
	@cp -r ../share  $(BASEDIR)
	@rm -rf `find $(BASEDIR)/share -name ".svn"`
	@mkdir $(BASEDIR)/share/FBReader/help
	@cp ../data/help/HowToStart.brownish.fb2 $(BASEDIR)/share/FBReader/help/HowToStart.fb2
	@mkdir $(BASEDIR)/share/FBReader/default
	@cp ../data/default/keymap.qtopia.xml $(BASEDIR)/share/FBReader/default/keymap.xml
	@cp ../data/default/styles.$(STYLE).xml $(BASEDIR)/share/FBReader/default/styles.xml
	@mkdir -p $(APPDIR)
	@cp $(UI_TYPE)/fbreader.desktop $(APPDIR)

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../..

.cleandir:
	@rm -rf data
