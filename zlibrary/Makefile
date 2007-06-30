ROOTDIR = $(CURDIR)/..
MAKEFILESDIR = $(ROOTDIR)/makefiles
include $(MAKEFILESDIR)/config.mk

ARCHSUBDIRS = zaurus

all:
	@$(MAKE) -C core
	@$(MAKE) -C text
	@$(MAKE) -C ui

do_install:
	@$(MAKE) -C core do_install
	@$(MAKE) -C text do_install
	@$(MAKE) -C ui do_install

package:
	@if [ -d $(TARGET_ARCH) ]; then \
		cd $(TARGET_ARCH); $(MAKE); \
	fi;

clean:
	@$(MAKE) -C core clean
	@$(MAKE) -C text clean
	@$(MAKE) -C ui clean
	@for target in $(ARCHSUBDIRS); do \
		$(MAKE) -C $$target clean; \
	done;
