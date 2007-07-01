VERSION = $(shell cat fbreader/VERSION)

.debian:
	@echo -en "Building $(ARCHITECTURE) debian package..."
	@mkdir $(TMPDIR)
	@cp -r Makefile zlibrary fbreader makefiles $(TMPDIR)
	@rm -rf `find $(TMPDIR) -name ".svn"`
	@mkdir $(TMPDIR)/debian
	@for file in distributions/$(ARCHITECTURE)-debian/*; do \
		sed -e "s#@VERSION@#$(VERSION)#g" $$file > $(TMPDIR)/debian/`basename $$file`; \
		chmod --reference $$file $(TMPDIR)/debian/`basename $$file`; \
	done
	@cd $(TMPDIR); dpkg-buildpackage -rfakeroot -us -uc 1> $(CURDIR)/$(ARCHITECTURE)-debian.log 2>&1; cd $(CURDIR)
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-debian.log
	@echo " OK"

.ipk:
	@echo -en "Building $(ARCHITECTURE) ipk package..."
	@make -f distributions/$(ARCHITECTURE)-ipk/rules build 1> $(ARCHITECTURE)-ipk.log 2>&1
	@for controlfile in distributions/$(ARCHITECTURE)-ipk/*.control; do \
		mkdir $(TMPDIR); \
		mkdir $(TMPDIR)/data; \
		make -f distributions/$(ARCHITECTURE)-ipk/rules DESTDIR=$(TMPDIR)/data install-`basename $$controlfile .control` 1>> $(ARCHITECTURE)-ipk.log 2>&1; \
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
	@echo " OK"
	@rm -f $(CURDIR)/$(ARCHITECTURE)-ipk.log
	@make -f distributions/$(ARCHITECTURE)-ipk/rules clean 1> /dev/null 2>&1;
