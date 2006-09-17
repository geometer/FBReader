ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

DEB_VERSION = 2.0

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

PACKAGENAME = FBReader-$(UI_TYPE)
FILE = $(PACKAGENAME).control

version = $(shell cat ../VERSION)
depends = $(shell sed -n -e "s/^Depends: *//p" $(FILE))
package = $(PACKAGENAME)_$(version)_$(TARGET_ARCH)_armv5tel.ipk

all: .tmpdir .copyfiles $(CTRLDIR)/control $(package) .rmtmpdir
	@mv $(package) ../..

install:
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/help
	@install -m 0644 ../data/help/HowToStart.brownish.640x480.fb2 $(DESTDIR)$(SHAREDIR)/FBReader/help/HowToStart.fb2
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/default
	@install -m 0644 ../data/default/config.pdaxrom.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/config.xml
	@install -m 0644 ../data/default/keymap.pdaxrom.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/keymap.xml
	@install -m 0644 ../data/default/styles.640x480.xml $(DESTDIR)$(SHAREDIR)/FBReader/default/styles.xml
	@install -d $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -m 0644 ../icons/34x28/FBReader.png $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -m 0644 $(wildcard ../icons/34x28/FBReader/*.png) $(DESTDIR)$(SHAREDIR)/pixmaps/FBReader
	@install -d $(DESTDIR)/$(INSTALLDIR)/apps/Office/FBReader
	@install -m 0644 data/apps/Office/FBReader/AppRun $(DESTDIR)$(INSTALLDIR)/apps/Office/FBReader
	@install -m 0644 data/apps/Office/FBReader/AppInfo.xml $(DESTDIR)$(INSTALLDIR)/apps/Office/FBReader
	@install -d $(DESTDIR)/$(SHAREDIR)/applications
	@install -m 0644 data/share/applications/FBReader.desktop $(DESTDIR)/$(SHAREDIR)/applications

.tmpdir:
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -rf $(TDIR)

.copyfiles:
	@cd ../..; make DESTDIR=$(PWD)/$(DATADIR) install

$(CTRLDIR)/control: $(FILE)
	@echo "Package: $(PACKAGENAME)" > $@
	@echo "Version: $(version)" >> $@
	@echo "Depends: $(depends)" >> $@
	@egrep -v "^(Depends):" $(FILE) >> $@

$(package):
	@echo -n "Packaging..."
	@echo "$(DEB_VERSION)" > $(TDIR)/debian-binary
	@tar czf $(TDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TDIR)/data.tar.gz -C $(DATADIR) .
	@rm -rf $(CTRLDIR) $(DATADIR)
	@tar czf $(package) -C $(TDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"

clean: .rmtmpdir
	@rm -vf *.ipk
