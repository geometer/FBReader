ROOTDIR = $(CURDIR)

include makefiles/platforms.mk

ZLIBDIRS = zlibrary/core zlibrary/text zlibrary/ui
APPDIRS = fbreader

all:
	@for dir in $(ZLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; \
			if ! $(MAKE); then \
				exit 1; \
			fi; \
			cd $(ROOTDIR); \
		fi; \
	done;

install: all do_install

do_install:
	@for dir in $(ZLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

do_install_dev:
	@for dir in $(ZLIBDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

clean:
	@for dir in $(ZLIBDIRS) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

distclean: clean
	@rm -rvf *.tgz *.tar.gz *.ipk *.deb *.prc *.exe *.log *.dsc *.changes *.rpm
	@rm -rf tmp packages packages-nonGPL fbreader-*
