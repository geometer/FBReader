ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/platforms.mk

BASEDIR = data/opt/QtPalmtop
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
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/apps/Applications
	@mkdir -p $(BASEDIR)/pics/fbreader
	@sed "s/VERSION/$(version)/" $(UI_TYPE)/control.in > $(DEBDIR)/control
	@cp ../FBReader $(BASEDIR)/bin
	@cp $(UI_TYPE)/fbreader.desktop $(BASEDIR)/apps/Applications
	@cp ../icons/22x18/FBReader.png $(BASEDIR)/pics/fbreader
	@cp ../icons/34x28/FBReader/*.png $(BASEDIR)/pics/fbreader
	@cp -r ../share  $(BASEDIR)
	@rm -rf `find $(BASEDIR)/share -name ".svn"`
	@mkdir $(BASEDIR)/share/FBReader/help
	@cp ../data/help/HowToStart.brownish.fb2 $(BASEDIR)/share/FBReader/help/HowToStart.fb2
	@mkdir $(BASEDIR)/share/FBReader/default
	@cp ../data/default/keymap.qtopia.xml $(BASEDIR)/share/FBReader/default/keymap.xml
	@cp ../data/default/styles.640x480.xml $(BASEDIR)/share/FBReader/default/styles.xml

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../..

.cleandir:
	@rm -rf data
