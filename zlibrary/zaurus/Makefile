ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

ARCH = arm

TMPDIR = tmp
DESTDIR = $(TMPDIR)/data
CTRLDIR = $(TMPDIR)/control

PACKAGENAME = libzlibrary
CONTROLFILE = $(PACKAGENAME).control

ifeq "$(UI_TYPE)" "qtopia-240x320"
  RESOLUTION = 240x320
else # UI_TYPE == qtopia-640x480
  RESOLUTION = 640x480
endif

version = $(shell cat ../VERSION)
depends = expat,libbz2
package = $(PACKAGENAME)_$(version)_$(RESOLUTION)_$(ARCH)

all: .ipk
	@mv *.ipk ../..

.ipk:
	@rm -rf $(TMPDIR)
	@mkdir -p $(DESTDIR) $(CTRLDIR)
	@cd ..; make DESTDIR=$(PWD)/$(DESTDIR) do_install
	@echo "Package: $(PACKAGENAME)" > $(CTRLDIR)/control
	@echo -n "Installed-Size: " >> $(CTRLDIR)/control
	@du -b -s $(DESTDIR) | sed -e 's/[	 ].*//' >> $(CTRLDIR)/control
	@echo "Filename: ./$(package).ipk" >> $(CTRLDIR)/control
	@echo "Version: $(version)" >> $(CTRLDIR)/control
	@echo "Depends: $(depends)" >> $(CTRLDIR)/control
	@cat $(CONTROLFILE) >> $(CTRLDIR)/control
	@echo -n "Packaging..."
	@echo "2.0" > $(TMPDIR)/debian-binary
	@tar czf $(TMPDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TMPDIR)/data.tar.gz -C $(DESTDIR) .
	@rm -rf $(CTRLDIR) $(DESTDIR)
	@tar czf $(package).ipk -C $(TMPDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"
	@rm -rf $(TMPDIR)

clean:
	@rm -rf $(TMPDIR)
	@rm -vf *.ipk
