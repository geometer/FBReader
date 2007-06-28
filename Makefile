ROOTDIR = $(CURDIR)

include makefiles/platforms.mk

ZLIBDIR = zlibrary
APPDIRS = fbreader GeometricCalculator SampleApplications/0 SampleApplications/1 SampleApplications/2

all:
	@for dir in $(ZLIBDIR) $(APPDIRS); do \
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
	@for dir in $(ZLIBDIR) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

packages: all
	@for dir in $(ZLIBDIR) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make package; cd $(ROOTDIR); \
		fi; \
	done

clean:
	@for dir in $(ZLIBDIR) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; make $@; cd $(ROOTDIR); \
		fi; \
	done

distclean: clean
	rm -vf *.tgz *.ipk *.deb *.prc *.exe *.log

archives: clean
