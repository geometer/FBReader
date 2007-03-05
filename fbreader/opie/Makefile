ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

DESTDIR = data
DEBDIR = $(DESTDIR)/DEBIAN

version = $(shell cat ../VERSION)
package = fbreader-opie_$(version)_$(TARGET_ARCH)_arm.ipk

all:
ifneq "$(TARGET_ARCH)" "qvfb"
	@make .builddir
	@make .buildpackage
	@make .cleandir
endif

install:

clean: .cleandir
	@rm -f *.ipk

.builddir: .cleandir
	@mkdir -p $(DEBDIR)
	@cd ../../zlibrary; make DESTDIR=$(PWD)/$(DESTDIR) do_install
	@cd ..; make DESTDIR=$(PWD)/$(DESTDIR) do_install
	@mkdir -p $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@mkdir -p $(DESTDIR)$(INSTALLDIR)/pics/fbreader
	@sed "s/VERSION/$(version)/" control.in > $(DEBDIR)/control
	@cp fbreader.desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@cp ../icons/22x18/FBReader.png $(DESTDIR)$(INSTALLDIR)/pics/fbreader
	@cp ../icons/22x18/FBReader/*.png $(DESTDIR)$(INSTALLDIR)/pics/fbreader
ifeq "$(TARGET_ARCH)" "opensimpad-0.9.0"
	@cp ../data/default/config.opie.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@cp ../data/default/keymap.opie-opensimpad.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@cp ../data/default/styles.640x480.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
endif

.buildpackage:
	@dpkg -b data $(package)
	@cp $(package) ../..

.cleandir:
	@rm -rf data
