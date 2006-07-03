DEB_VERSION = 2.0

ARCH = arm

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

PACKAGENAME = FBReader
CONTROLFILE = $(PACKAGENAME).$(RESOLUTION).control

version = $(shell cat ../VERSION)
depends = $(shell sed -n -e "s/^Depends: *//p" $(CONTROLFILE))
files = $(shell sed -n -e "s/^Files://p" $(CONTROLFILE))
package = $(PACKAGENAME)_$(version)_$(RESOLUTION)_$(ARCH)

all:
	@RESOLUTION=240x320 $(MAKE) ipk
	@RESOLUTION=640x480 $(MAKE) ipk

ipk: .tmpdir .copyfiles $(CTRLDIR)/control package .rmtmpdir
	@cp $(package).ipk ../..

install:

.tmpdir:
	@ln -sf .. bin
	@ln -sf ../icons/34x28 pics144
	@ln -sf ../icons/18x15 pics
	@cp -r ../share .
	@mkdir share/zlibrary
	@cp ../../zlibrary/data/keynames.zaurus.xml share/zlibrary/keynames.xml
	@cp -r ../../zlibrary/data/encodings share/zlibrary
	@mkdir share/FBReader/help
	@cp ../data/help/HowToStart.brownish.$(RESOLUTION).fb2 share/FBReader/help/HowToStart.fb2
	@mkdir share/FBReader/default
	@cp ../data/default/keymap.zaurus.xml share/FBReader/default/keymap.xml
	@cp ../data/default/styles.$(RESOLUTION).xml share/FBReader/default/styles.xml
	@mkdir -p apps/Applications
	@cp FBReader.$(RESOLUTION).desktop apps/Applications/FBReader.desktop
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -f bin pics pics144
	@rm -rf apps share $(TDIR)

.copyfiles:
	@for f in $(files); do \
		if [ -d $f ]; then \
			ffiles=`find $$f -type f -o -type b -o -type c -o -type l | grep -v .svn`; \
  	else \
  		ffiles=$$f; \
  	fi; \
  	for ff in $$ffiles; do \
  		if [ -f $$ff -o -b $$ff -o -c $$ff ]; then \
				fdir=$(DATADIR)/opt/QtPalmtop/`dirname $$ff`; \
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
	@echo -n "Installed-Size: " >> $@
	@du -b -s $(DATADIR) | sed -e 's/[	 ].*//' >> $@
	@echo "Filename: ./$(package).ipk" >> $@
	@echo "Version: $(version)" >> $@
	@echo "Depends: $(depends)" >> $@
	@egrep -v "^(Files|Version|Depends):" $(CONTROLFILE) >> $@

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
