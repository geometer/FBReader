include makefiles/target.mk

ALLAPPDIRS = zmahjongg zsolitaire fbreader GeometricCalculator

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	APPDIRS = zmahjongg zsolitaire fbreader GeometricCalculator
else
	APPDIRS = fbreader
endif

LIBDIR = zlibrary

all:
	@for dir in $(LIBDIR) $(APPDIRS); do \
		cd $$dir; \
		if ! make; then \
			exit 1; \
		fi; \
		cd ..; \
	done

clean:
	@for dir in $(LIBDIR) $(APPDIRS); do \
		cd $$dir; \
		make clean; \
		cd ..; \
	done

distclean: clean
	rm -vf *.tgz

archives: clean
	@for dir in $(ALLAPPDIRS); do \
		echo -n "Creating archive for $$dir..."; \
		tar czf $$dir-`cat $$dir/VERSION`.tgz --exclude .svn  $$dir $(LIBDIR) makefiles; \
		echo " OK"; \
	done
