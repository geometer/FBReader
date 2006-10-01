ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

DEB_VERSION = 2.0

ARCH = arm

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

PACKAGENAME = FBReader
CONTROLFILE = $(PACKAGENAME).control

version = $(shell cat ../VERSION)
depends = expat,libenca,libbz2
package = $(PACKAGENAME)_$(version)_$(RESOLUTION)_$(ARCH)

all:
	@$(MAKE) RESOLUTION=240x320 .ipk
	@$(MAKE) RESOLUTION=640x480 .ipk
	@mv *.ipk ../..

ifeq "$(RESOLUTION)" "240x320"
  PICS_FROM = 18x15
  PICS_TO = pics
else # RESOLUTION == 640x480
  PICS_FROM = 34x28
  PICS_TO = pics144
endif

install:
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.$(RESOLUTION).fb2 $(DESTDIR)$(SHAREDIR)/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/default
	@install -m 0644 ../data/default/config.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.$(RESOLUTION).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -m 0644 ../icons/$(PICS_FROM)/FBReader.png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader
	@install -m 0644 $(wildcard ../icons/$(PICS_FROM)/FBReader/*.png) $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 FBReader.$(RESOLUTION).desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/FBReader.desktop

.ipk:
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)
	@cd ../..; make DESTDIR=$(PWD)/$(DATADIR) install
	@echo "Package: $(PACKAGENAME)" > $(CTRLDIR)/control
	@echo -n "Installed-Size: " >> $(CTRLDIR)/control
	@du -b -s $(DATADIR) | sed -e 's/[	 ].*//' >> $(CTRLDIR)/control
	@echo "Filename: ./$(package).ipk" >> $(CTRLDIR)/control
	@echo "Version: $(version)" >> $(CTRLDIR)/control
	@echo "Depends: $(depends)" >> $(CTRLDIR)/control
	@cat $(CONTROLFILE) >> $(CTRLDIR)/control
	@echo -n "Packaging..."
	@echo "$(DEB_VERSION)" > $(TDIR)/debian-binary
	@tar czf $(TDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TDIR)/data.tar.gz -C $(DATADIR) .
	@rm -rf $(CTRLDIR) $(DATADIR)
	@tar czf $(package).ipk -C $(TDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"
	@rm -rf $(TDIR)

clean:
	@rm -rf $(TDIR)
	@rm -vf *.ipk
