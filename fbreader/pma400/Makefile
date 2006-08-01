DEB_VERSION = 2.0

ARCH = arm

TDIR = __ipk

DATADIR = $(TDIR)/data
CTRLDIR = $(TDIR)/control

CONTROLFILE = $(wildcard *.control)
PACKAGENAME = $(patsubst %.control,%,$(CONTROLFILE))

version = $(shell cat ../VERSION)
depends = $(shell sed -n -e "s/^Depends: *//p" $(CONTROLFILE))
files = $(shell sed -n -e "s/^Files://p" $(CONTROLFILE))
package = $(PACKAGENAME)_$(version)_pma400_$(ARCH)

all: .tmpdir .copyfiles $(CTRLDIR)/control package .rmtmpdir
	@cp $(package).ipk ../..

install:

.tmpdir:
	@mkdir bin
	@cp ../FBReader bin
	@mkdir lib
	@cp ../../zlibrary/libzlibrary.so lib
	@cp ../../zldictionary/libzldictionary.so lib
	@cp -r ../icons/18x15 pics
	@cp -r ../share .
	@mkdir share/zlibrary
	@cp ../../zlibrary/data/keynames.pma400.xml share/zlibrary/keynames.xml
	@cp -r ../../zlibrary/data/encodings share/zlibrary
	@mkdir share/FBReader/help
	@cp ../data/help/HowToStart.brownish.240x320.fb2 share/FBReader/help/HowToStart.fb2
	@mkdir share/FBReader/default
	@cp ../data/default/config.pma400.xml share/FBReader/default/config.xml
	@cp ../data/default/keymap.pma400.xml share/FBReader/default/keymap.xml
	@cp ../data/default/styles.240x320.xml share/FBReader/default/styles.xml
	@rm -rf $(TDIR)
	@mkdir -p $(DATADIR) $(CTRLDIR)

.rmtmpdir:
	@rm -rf bin lib pics share $(TDIR)

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
