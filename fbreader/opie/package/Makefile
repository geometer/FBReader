include ../../../makefiles/platforms.mk

BASEDIR = data/opt/QtPalmtop
DEBDIR = data/DEBIAN

version = $(shell cat ../../VERSION)
package = fbreader-opie_$(version)_opensimpad-0.9.0_arm.ipk

all:
ifeq "$(TARGET_ARCH)" "opensimpad-opie"
	@make .builddir
	@make .buildpackage
	@make .cleandir
endif

clean: .cleandir
	@rm -f *.ipk

.builddir: .cleandir
	@mkdir -p $(DEBDIR)
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/apps/Applications
	@mkdir -p $(BASEDIR)/pics/fbreader
	@cp control $(DEBDIR)
	@cp ../../FBReader $(BASEDIR)/bin
	@cp fbreader.desktop $(BASEDIR)/apps/Applications
	@cp ../../icons/22x18/FBReader.png $(BASEDIR)/pics/fbreader
	@cp ../../icons/22x18/FBReader/*.png $(BASEDIR)/pics/fbreader
	@cp -r ../../share  $(BASEDIR)
	@rm -rf `find $(BASEDIR)/share -name ".svn"`
	@mkdir $(BASEDIR)/share/FBReader/help
	@cp ../../data/help/HowToStart.brownish.fb2 $(BASEDIR)/share/FBReader/help/HowToStart.fb2
	@mkdir $(BASEDIR)/share/FBReader/default
	@cp ../../data/default/keymap.opie-opensimpad.xml $(BASEDIR)/share/FBReader/default/keymap.xml

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../../..


.cleandir:
	@rm -rf data
