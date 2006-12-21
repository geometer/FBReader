ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

ARCH = arm

TMPDIR = tmp
DESTDIR = $(TMPDIR)/data
CTRLDIR = $(TMPDIR)/control

PACKAGENAME = FBReader
CONTROLFILE = $(PACKAGENAME).control

ifeq "$(UI_TYPE)" "qtopia-240x320"
  RESOLUTION = 240x320
  PICS_FROM = 18x15
  PICS_TO = pics
else # UI_TYPE == qtopia-640x480
  RESOLUTION = 640x480
  PICS_FROM = 34x28
  PICS_TO = pics144
endif

version = $(shell cat ../VERSION)
depends = libzlibrary-$(UI_TYPE),libenca
package = $(PACKAGENAME)_$(version)_$(RESOLUTION)_$(ARCH)

all: .ipk
	@mv *.ipk ../..

install:
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.$(RESOLUTION).fb2 $(DESTDIR)$(SHAREDIR)/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/default
	@install -m 0644 ../data/default/config.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/dictionary-integration.zaurus.xml $(DESTDIR)$(SHAREDIR)/FBReader/dictionary-integration.xml
	@install -m 0644 ../data/default/styles.$(RESOLUTION).xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -m 0644 ../icons/$(PICS_FROM)/FBReader.png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader
	@install -m 0644 $(wildcard ../icons/$(PICS_FROM)/FBReader/*.png) $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 FBReader.$(RESOLUTION).desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/FBReader.desktop

.ipk:
	@rm -rf $(TMPDIR)
	@mkdir -p $(DESTDIR) $(CTRLDIR)
	@cd ..; make DESTDIR=$(PWD)/$(DESTDIR) do_install
	@echo "Package: $(PACKAGENAME)" > $(CTRLDIR)/control
	@echo -n "Installed-Size: " >> $(CTRLDIR)/control
	@du -b -s $(DESTDIR) | sed -e 's/[	 ].*//' >> $(CTRLDIR)/control
	@echo "Filename: ./$(package).ipk" >> $(CTRLDIR)/control
	@echo "Version: $(version)" >> $(CTRLDIR)/control
	@echo "Depends: $(depends)" >> $(CTRLDIR)/control
	@cat $(CONTROLFILE) >> $(CTRLDIR)/control
	@echo -n "Packaging..."
	@echo "2.0" > $(TMPDIR)/debian-binary
	@tar czf $(TMPDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TMPDIR)/data.tar.gz -C $(DESTDIR) .
	@rm -rf $(CTRLDIR) $(DESTDIR)
	@tar czf $(package).ipk -C $(TMPDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"
	@rm -rf $(TMPDIR)

clean:
	@rm -rf $(TMPDIR)
	@rm -vf *.ipk
