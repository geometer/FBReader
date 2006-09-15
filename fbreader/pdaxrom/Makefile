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
files = $(shell sed -n -e "s/^Files://p" $(FILE))
package = $(PACKAGENAME)_$(version)_$(TARGET_ARCH)_armv5tel.ipk

all: .tmpdir .copyfiles $(CTRLDIR)/control $(package) .rmtmpdir
	@mv $(package) ../..

install:

.tmpdir:
	@mkdir -p usr
	@mkdir -p usr/apps/Office/FBReader
	@cp data/apps/Office/FBReader/AppRun usr/apps/Office/FBReader
	@cp data/apps/Office/FBReader/AppInfo.xml usr/apps/Office/FBReader
	@mkdir -p usr/share/applications
	@cp data/share/applications/FBReader.desktop usr/share/applications
	@mkdir -p .$(BINDIR)
	@cp ../FBReader .$(BINDIR)
	@cp -r ../share/FBReader usr/share
	@mkdir -p usr/share/FBReader/help
	@cp ../data/help/HowToStart.brownish.640x480.fb2 usr/share/FBReader/help/HowToStart.fb2
	@mkdir -p usr/share/FBReader/default
	@cp ../data/default/config.pdaxrom.xml usr/share/FBReader/default/config.xml
	@cp ../data/default/keymap.pdaxrom.xml usr/share/FBReader/default/keymap.xml
	@cp ../data/default/styles.640x480.xml usr/share/FBReader/default/styles.xml
	@mkdir -p usr/share/pixmaps/FBReader
	@cp `find ../icons/34x28 -name "*.png"` usr/share/pixmaps/FBReader
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -rf usr $(TDIR)

.copyfiles:
	@for f in $(files); do \
		if [ -d $f ]; then \
			ffiles=`find $$f -type f -o -type b -o -type c -o -type l | grep -v .svn`; \
  	else \
  		ffiles=$$f; \
  	fi; \
  	for ff in $$ffiles; do \
  		if [ -f $$ff -o -b $$ff -o -c $$ff ]; then \
				fdir=$(DATADIR)/`dirname $$ff`; \
				mkdir -p $$fdir; \
				cp $$ff $$fdir; \
			else \
  			echo >&2 "No such file: $$ff"; \
				exit 1; \
  		fi; \
  	done; \
  done;
	@cd ../..; make DESTDIR=$(PWD)/$(DATADIR) install

$(CTRLDIR)/control: $(FILE)
	@echo "Package: $(PACKAGENAME)" > $@
	@echo "Version: $(version)" >> $@
	@echo "Depends: $(depends)" >> $@
	@egrep -v "^(Files|Depends):" $(FILE) >> $@

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
