ROOTDIR = $(PWD)/../..
include $(ROOTDIR)/makefiles/config.mk

ARCH = arm

TMPDIR = tmp
DESTDIR = $(TMPDIR)/data
CTRLDIR = $(TMPDIR)/control

PACKAGENAME = GeometricCalculator
CONTROLFILE = $(PACKAGENAME).control

RESOLUTION = 640x480
PICS_TO = pics144

version = $(shell cat ../VERSION)
depends = libzlibrary
package = $(PACKAGENAME)_$(version)_$(RESOLUTION)_$(ARCH)

all: package
	@mv *.ipk ../..

package:
	@cd ..; DESTDIR=$(PWD)/$(DESTDIR) $(MAKE) do_install
	@mkdir -p $(CTRLDIR)
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -m 0644 ../icons/640x480/GeometricCalculator.png $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)
	@install -d $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/GeometricCalculator
	@install -m 0644 $(wildcard ../icons/640x480/GeometricCalculator/*.png) $(DESTDIR)$(INSTALLDIR)/$(PICS_TO)/GeometricCalculator
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 GeometricCalculator.desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications
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
