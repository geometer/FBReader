ROOTDIR = $(PWD)/../../..
include $(ROOTDIR)/makefiles/config.mk

ARCH = arm

TMPDIR = tmp
DESTDIR = $(TMPDIR)/data
CTRLDIR = $(TMPDIR)/control

TARGET = SampleApplication

RESOLUTION = 640x480
PICS_TO = pics144

version = $(shell cat ../VERSION)
depends = libzlibrary-$(UI_TYPE)
package = $(TARGET)_$(version)_$(RESOLUTION)_$(ARCH).ipk

all: package
	@mv $(package) $(ROOTDIR)

package:
	@cd ..; DESTDIR=$(PWD)/$(DESTDIR) $(MAKE) do_install
	@mkdir -p $(CTRLDIR)
	@install -d $(DESTDIR)$(INSTALLDIR)/apps/Applications
	@install -m 0644 desktop $(DESTDIR)$(INSTALLDIR)/apps/Applications/$(TARGET).desktop
	@echo "Package: $(TARGET)" > $(CTRLDIR)/control
	@echo -n "Installed-Size: " >> $(CTRLDIR)/control
	@du -b -s $(DESTDIR) | sed -e 's/[	 ].*//' >> $(CTRLDIR)/control
	@echo "Filename: ./$(package)" >> $(CTRLDIR)/control
	@echo "Version: $(version)" >> $(CTRLDIR)/control
	@echo "Depends: $(depends)" >> $(CTRLDIR)/control
	@cat control >> $(CTRLDIR)/control
	@echo -n "Packaging..."
	@echo "2.0" > $(TMPDIR)/debian-binary
	@tar czf $(TMPDIR)/control.tar.gz -C $(CTRLDIR) .
	@tar czf $(TMPDIR)/data.tar.gz -C $(DESTDIR) .
	@rm -rf $(CTRLDIR) $(DESTDIR)
	@tar czf $(package) -C $(TMPDIR) ./debian-binary ./control.tar.gz ./data.tar.gz
	@echo " OK"
	@rm -rf $(TMPDIR)

clean:
	@rm -rf $(TMPDIR)
	@rm -vf *.ipk
