ROOTDIR = $(CURDIR)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

ARCHSUBDIRS = zaurus maemo

default:

do_install:
	@$(MAKE) -C core do_install
	@$(MAKE) -C text do_install
	@$(MAKE) -C ui do_install

package:
	@if [ -d $(TARGET_ARCH) ]; then \
		cd $(TARGET_ARCH); $(MAKE); \
	fi;

clean:
	@for target in $(ARCHSUBDIRS); do \
		$(MAKE) -C $$target clean; \
	done;
