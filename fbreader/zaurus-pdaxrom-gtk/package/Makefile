include ../../../makefiles/platforms.mk

DEB_VERSION = 2.0

ifeq "$(PDAXROM_VERSION)" "rc11"
ARCH = pdaxrom_rc11_arm
else
ARCH = pdaxrom_arm
endif

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

FILE = $(wildcard *.control)
PACKAGENAME = $(patsubst %.control, %, $(FILE))

version = $(shell cat ../../VERSION)
depends = $(shell sed -n -e "s/^Depends: *//p" $(FILE))
files = $(shell sed -n -e "s/^Files://p" $(FILE))
package = $(PACKAGENAME)_$(version)_$(ARCH)

all: .tmpdir .copyfiles $(CTRLDIR)/control package .rmtmpdir
	@cp $(package).ipk ../../..

.tmpdir:
	@cp ../../FBReader usr/bin
	@cp -r ../../share/FBReader usr/share
	@mkdir -p usr/share/FBReader/help
	@cp ../../data/help/HowToStart.brownish.fb2 usr/share/FBReader/help/HowToStart.fb2
	@mkdir -p usr/share/FBReader/default
	@cp ../../data/default/keymap.pdaxrom.xml usr/share/FBReader/default/keymap.xml
	@mkdir -p usr/share/pixmaps/FBReader
	@cp `find ../../icons/34x28 -name "*.png"` usr/share/pixmaps/FBReader
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -rf usr/bin/FBReader usr/share/pixmaps/FBReader usr/share/FBReader $(TDIR)

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

$(CTRLDIR)/control:
	@echo "Package: $(PACKAGENAME)" > $@
	@echo "Version: $(version)" >> $@
	@echo "Depends: $(depends)" >> $@
	@egrep -v "^(Files|Depends):" $(FILE) >> $@

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
