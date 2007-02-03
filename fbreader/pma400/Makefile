ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

DEB_VERSION = 2.0

ARCH = arm

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

CONTROLFILE = $(wildcard *.control)
PACKAGENAME = $(patsubst %.control,%,$(CONTROLFILE))

version = $(shell cat ../VERSION)
depends = expat,libenca,libbz2
package = $(PACKAGENAME)_$(version)_pma400_$(ARCH)

all: .tmpdir .copyfiles $(CTRLDIR)/control package .rmtmpdir
	@mv $(package).ipk ../..

install:
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.240x320.fb2 $(DESTDIR)$(SHAREDIR)/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/default
	@install -m 0644 ../data/default/config.pma400.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.pma400.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.240x320.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(INSTALLDIR)/pics
	@install -m 0644 ../icons/18x15/FBReader.png $(DESTDIR)$(INSTALLDIR)/pics
	@install -d $(DESTDIR)$(INSTALLDIR)/pics/FBReader
	@install -m 0644 $(wildcard ../icons/18x15/FBReader/*.png) $(DESTDIR)$(INSTALLDIR)/pics/FBReader
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 FBReader.desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/FBReader.desktop

.tmpdir:
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -rf $(TDIR)

.copyfiles:
	@cd ../../zlibrary; make DESTDIR=$(PWD)/$(TMPDIR) do_install
	@cd ..; make DESTDIR=$(PWD)/$(TMPDIR) do_install

$(CTRLDIR)/control:
	@echo "Package: $(PACKAGENAME)" > $@
	@echo -n "Installed-Size: " >> $@
	@du -b -s $(DATADIR) | sed -e 's/[	 ].*//' >> $@
	@echo "Filename: ./$(package).ipk" >> $@
	@echo "Version: $(version)" >> $@
	@echo "Depends: $(depends)" >> $@
	@cat $(CONTROLFILE) >> $@

package:
	@echo -n "Packaging..."
	@echo "$(DEB_VERSION)" > $(TDIR)/debian-binary
	@tar czf $(TDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TDIR)/data.tar.gz -C $(DATADIR) .
	@rm -rf $(CTRLDIR) $(DATADIR)
	@tar czf $(package).ipk -C $(TDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"

clean: .rmtmpdir
	@rm -vf *.ipk
