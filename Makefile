ROOTDIR = $(PWD)

include makefiles/platforms.mk

ALLAPPDIRS = fbreader zmahjongg zsolitaire GeometricCalculator

APPDIRS = fbreader

ifeq "$(TARGET_ARCH)" "zaurus-qtopia"
	APPDIRS += zmahjongg zsolitaire GeometricCalculator
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	APPDIRS += GeometricCalculator
endif

LIBDIR = zlibrary

all:
	@for dir in $(LIBDIR) $(APPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; \
			if ! make; then \
				exit 1; \
			fi; \
			cd ..; \
		fi; \
	done

clean:
	@for dir in $(LIBDIR) $(ALLAPPDIRS); do \
		if [ -d $$dir ]; then \
			cd $$dir; \
			make clean; \
			cd ..; \
		fi; \
	done

distclean: clean
	rm -vf *.tgz *.ipk *.prc

archives: clean
	@for dir in $(ALLAPPDIRS); do \
		echo -n "Creating archive for $$dir..."; \
		tar czf $$dir-`cat $$dir/VERSION`.tgz --exclude .svn  $$dir $(LIBDIR) makefiles; \
		echo " OK"; \
	done
