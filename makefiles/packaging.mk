.debian:
	@echo -en "Building $(ARCHITECTURE) debian package..."
	@mkdir $(TMPDIR)
	@cp -r Makefile zlibrary fbreader makefiles $(TMPDIR)
	@rm -rf `find $(TMPDIR) -name ".svn"`
	@cp -r distributions/$(ARCHITECTURE)-debian $(TMPDIR)/debian
	@cd $(TMPDIR); dpkg-buildpackage -rfakeroot -us -uc 1> $(CURDIR)/$(ARCHITECTURE)-debian.log 2>&1; cd $(CURDIR)
	@rm -rf $(TMPDIR)
	@rm $(CURDIR)/$(ARCHITECTURE)-debian.log
	@echo " OK"
