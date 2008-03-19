VERSION = $(shell cat fbreader/VERSION)
SOVERSION = $(shell cat zlibrary/SOVERSION)
TMPDIR = $(CURDIR)/fbreader-$(VERSION)

motopkg:
	@echo -n "Building $(ARCHITECTURE) $@ package..."
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-$@.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) package 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-$@.log
	@echo " OK"

tarball:
	@echo -n "Building $(ARCHITECTURE) $@ package..."
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-$@.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) package 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-$@.log
	@echo " OK"

debian:
	@echo -n "Building $(ARCHITECTURE) debian package..."
	@mkdir $(TMPDIR)
	@cp -a Makefile zlibrary fbreader makefiles $(TMPDIR)
	@rm -rf `find $(TMPDIR) -name ".svn"`
	@mkdir $(TMPDIR)/debian
	@for file in $(DIST_DIR)/debian/$(ARCHITECTURE)/*; do \
		sed -e "s#@SOVERSION@#$(SOVERSION)#g" $$file | \
		sed -e "s#@VERSION@#$(VERSION)#g" > $(TMPDIR)/debian/`basename $$file`; \
		chmod --reference $$file $(TMPDIR)/debian/`basename $$file`; \
	done
	@cd $(TMPDIR); dpkg-buildpackage -rfakeroot -us -uc 1> $(CURDIR)/$(ARCHITECTURE)-debian.log 2>&1; cd $(CURDIR)
	#@rm -rf $(TMPDIR)
	#@rm -f $(CURDIR)/$(ARCHITECTURE)-debian.log
	@echo " OK"

ipk:
	@echo -n "Building $(ARCHITECTURE) ipk package..."
	@make -f $(DIST_DIR)/ipk/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-ipk.log 2>&1
	@for controlfile in $(DIST_DIR)/ipk/$(ARCHITECTURE)/*.control; do \
		controlname=`basename $$controlfile .control`; \
		mkdir $(TMPDIR); \
		mkdir $(TMPDIR)/data; \
		make -f $(DIST_DIR)/ipk/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR)/data install-$$controlname 1>> $(ARCHITECTURE)-ipk.log 2>&1; \
		sed \
			-e "s#@VERSION@#$(VERSION)#" \
			-e "s#@SIZE@#`du -s -b $(TMPDIR)/data | cut -f1`#" \
			$$controlfile > $(TMPDIR)/control; \
		tar czf $(TMPDIR)/control.tar.gz -C $(TMPDIR) ./control; \
		tar czf $(TMPDIR)/data.tar.gz -C $(TMPDIR)/data .; \
		echo 2.0 > $(TMPDIR)/debian-binary; \
		tar czf `egrep ^Filename: $(TMPDIR)/control | cut -d' ' -f2` -C $(TMPDIR) ./debian-binary ./control.tar.gz ./data.tar.gz; \
		rm -rf $(TMPDIR); \
	done
	@make -f $(DIST_DIR)/ipk/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1;
	@rm -f $(CURDIR)/$(ARCHITECTURE)-ipk.log
	@echo " OK"

debipk:
	@echo -n "Building $(ARCHITECTURE) debipk package..."
	@make -f $(DIST_DIR)/debipk/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-debipk.log 2>&1
	@for controlfile in $(DIST_DIR)/debipk/$(ARCHITECTURE)/*.control; do \
		controlname=`basename $$controlfile .control`; \
		mkdir $(TMPDIR); \
		make -f $(DIST_DIR)/debipk/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install-$$controlname 1>> $(ARCHITECTURE)-debipk.log 2>&1; \
		mkdir $(TMPDIR)/DEBIAN; \
		sed \
			-e "s#@VERSION@#$(VERSION)#" \
			-e "s#@SIZE@#`du -s -b $(TMPDIR) | cut -f1`#" \
			$$controlfile > $(TMPDIR)/DEBIAN/control; \
		dpkg -b $(TMPDIR) `sed "s#@VERSION@#$(VERSION)#" $(DIST_DIR)/debipk/$(ARCHITECTURE)/$$controlname.name` 1>> $(ARCHITECTURE)-debipk.log 2>&1; \
		rm -rf $(TMPDIR); \
	done
	@make -f $(DIST_DIR)/debipk/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -f $(CURDIR)/$(ARCHITECTURE)-debipk.log
	@echo " OK"

nsi:
	@echo -n "Building $(ARCHITECTURE) nsi package..."
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-nsi.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-nsi.log 2>&1
	@sed "s#@VERSION@#$(VERSION)#" $(DIST_DIR)/nsi/$(ARCHITECTURE)/control.nsi > $(TMPDIR)/control.nsi
	@cd $(TMPDIR); makensis control.nsi 1>> $(CURDIR)/$(ARCHITECTURE)-nsi.log 2>&1
	@mv $(TMPDIR)/*.exe .
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-nsi.log
	@echo " OK"
