VERSION = $(shell cat fbreader/VERSION)
SOVERSIONCORE = $(shell cat zlibrary/core/SOVERSION)
SOVERSIONTEXT = $(shell cat zlibrary/text/SOVERSION)
TMPDIR = $(CURDIR)/fbreader-$(VERSION)

motopkg:
	@echo -n 'Building $(ARCHITECTURE) $@ package...'
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-$@.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) package 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-$@.log
	@echo ' OK'

tarball:
	@echo -n 'Building $(ARCHITECTURE) $@ package...'
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-$@.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) package 1>> $(ARCHITECTURE)-$@.log 2>&1
	@make -f $(DIST_DIR)/$@/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-$@.log
	@echo ' OK'

debian:
	@echo -n 'Building $(ARCHITECTURE) debian package...'
	@mkdir $(TMPDIR)
	@cp -a Makefile zlibrary fbreader makefiles ChangeLog $(TMPDIR)
	@rm -rf `find $(TMPDIR) -name ".svn"`
	@mkdir $(TMPDIR)/debian
	@for file in $(DIST_DIR)/debian/$(ARCHITECTURE)/*; do \
		if [ -f $$file ]; then \
			sed -e "s#@SOVERSIONCORE@#$(SOVERSIONCORE)#g" $$file | \
			sed -e "s#@SOVERSIONTEXT@#$(SOVERSIONTEXT)#g" | \
			sed -e "s#@VERSION@#$(VERSION)#g" > $(TMPDIR)/debian/`basename $$file`; \
			chmod --reference $$file $(TMPDIR)/debian/`basename $$file`; \
		else \
			cp -r $$file $(TMPDIR)/debian/`basename $$file`; \
		fi; \
	done
	@cd $(TMPDIR); dpkg-buildpackage -rfakeroot -us -uc 1> $(CURDIR)/$(ARCHITECTURE)-debian.log 2>&1; cd $(CURDIR)
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-debian.log
	@echo ' OK'

ipk:
	@echo -n 'Building $(ARCHITECTURE) ipk package...'
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
	@echo ' OK'

debipk:
	@echo -n 'Building $(ARCHITECTURE) debipk package...'
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
	@echo ' OK'

nsi:
	@echo -n 'Building $(ARCHITECTURE) nsi package...'
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules build 1> $(ARCHITECTURE)-nsi.log 2>&1
	@mkdir $(TMPDIR)
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules DESTDIR=$(TMPDIR) install 1>> $(ARCHITECTURE)-nsi.log 2>&1
	@sed "s#@VERSION@#$(VERSION)#" $(DIST_DIR)/nsi/$(ARCHITECTURE)/control.nsi > $(TMPDIR)/control.nsi
	@cd $(TMPDIR); makensis control.nsi 1>> $(CURDIR)/$(ARCHITECTURE)-nsi.log 2>&1
	@mv $(TMPDIR)/*.exe .
	@make -f $(DIST_DIR)/nsi/$(ARCHITECTURE)/rules clean 1> /dev/null 2>&1
	@rm -rf $(TMPDIR)
	@rm -f $(CURDIR)/$(ARCHITECTURE)-nsi.log
	@echo ' OK'

sis:
	@echo "OK"	
	cp -rf ../libs/ ./
	@make
	cd fbreader && mv FBReader FBReader.sym  && \
	/usr/share/qt4/qt-symbian/bin/elf2e32_qtwrapper --version 4.1793 --sid=0xE87CC83C --uid1=0x1000007a --uid2=0x100039CE --uid3=0xE87CC83C --targettype=EXE --elfinput=./FBReader.sym --output=./FBReader.exe --linkas=FBReader\{00040701\}\[e87cc83c\].exe --heap=0x20000,0x2000000 --stack=0x14000 --libpath=/usr/s60-sdk/epoc32/release/armv5/udeb/ --libpath=/usr/s60-sdk//epoc32/release/armv5/lib --libpath=/usr/s60-sdk/epoc32/release/armv5/urel --libpath=/usr/share/qt4/qt-symbian/lib --libpath=/usr/s60-sdk/epoc32/release/armv5/udeb --libpath=../zlibrary --libpath=../libs/symbian --capability=NetworkServices+ReadUserData+WriteUserData --dlldata --fpu=softvfp --unfrozen --compressionmethod bytepair --unpaged
	mifconv ./FBReader.mif /c32 fbreader/data/icons/application/symbian.svg
	cp distributions/sis/symbian/* ./
	cpp -nostdinc -undef -I/usr/share/qt4/qt-symbian/include/QtCore -I/usr/share/qt4/qt-symbian/include/QtGui -I/usr/share/qt4/qt-symbian/include -I/usr/s60-sdk/epoc32/include/ -I/usr/s60-sdk//epoc32/include/variant -I/usr/s60-sdk//epoc32/include/stdapis -I/usr/s60-sdk//epoc32/include/gcce -I/usr/s60-sdk/epoc32/include/stdapis/sys -I/usr/s60-sdk/epoc32/include/stdapis/stlport -I../zlibrary/core/include -I../zlibrary/text/include -I../libs/symbian/include -I/usr/s60-sdk/epoc32/include/osextensions/stdapis/stlport -I/usr/s60-sdk/epoc32/include/stdapis/stlport -I/usr/s60-sdk/epoc32/include -I/usr/s60-sdk/epoc32/include/oem -I/usr/s60-sdk/epoc32/include/middleware -I/usr/s60-sdk/epoc32/include/domain/middleware -I/usr/s60-sdk/epoc32/include/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc/sc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc/sc -I/usr/share/qt4/qt-symbian/mkspecs/features/symbian/../../common/symbian/header-wrappers -I . -DUNICODE -DQT_KEYPAD_NAVIGATION -DQT_SOFTKEYS_ENABLED -DQT_USE_MATH_H_FLOATS -D__PRODUCT_INCLUDE__=\</usr/s60-sdk/epoc32/include/variant/symbian_os.hrh\> -D__SYMBIAN32__ -D__MARM_INTERWORK__ -D_UNICODE -D__S60_50__ -D__S60_3X__ -D__SERIES60_3X__ -D__EPOC32__ -D__MARM__ -D__EABI__ -D__MARM_ARMV5__ -D__SUPPORT_CPP_EXCEPTIONS__ -D__GCCE__ -DUNICODE -D_STLP_NO_EXCEPTION_HEADER -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -D__EXE__ FBReader.rss -o ./FBReader.rpp && rcomp -u -m045,046,047 -s./FBReader.rpp -o./FBReader.rsc -h./FBReader.rsg -iFBReader.rss

	cpp -nostdinc -undef -I/usr/share/qt4/qt-symbian/include/QtCore -I/usr/share/qt4/qt-symbian/include/QtGui -I/usr/share/qt4/qt-symbian/include -I/usr/s60-sdk/epoc32/include/ -I/usr/s60-sdk//epoc32/include/variant -I/usr/s60-sdk//epoc32/include/stdapis -I/usr/s60-sdk//epoc32/include/gcce -I/usr/s60-sdk/epoc32/include/stdapis/sys -I/usr/s60-sdk/epoc32/include/stdapis/stlport -I../zlibrary/core/include -I../zlibrary/text/include -I../libs/symbian/include -I/usr/s60-sdk/epoc32/include/osextensions/stdapis/stlport -I/usr/s60-sdk/epoc32/include/stdapis/stlport -I/usr/s60-sdk/epoc32/include -I/usr/s60-sdk/epoc32/include/oem -I/usr/s60-sdk/epoc32/include/middleware -I/usr/s60-sdk/epoc32/include/domain/middleware -I/usr/s60-sdk/epoc32/include/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc/sc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc/sc -I/usr/share/qt4/qt-symbian/mkspecs/features/symbian/../../common/symbian/header-wrappers -I . -DUNICODE -DQT_KEYPAD_NAVIGATION -DQT_SOFTKEYS_ENABLED -DQT_USE_MATH_H_FLOATS -D__PRODUCT_INCLUDE__=\</usr/s60-sdk/epoc32/include/variant/symbian_os.hrh\> -D__SYMBIAN32__ -D__MARM_INTERWORK__ -D_UNICODE -D__S60_50__ -D__S60_3X__ -D__SERIES60_3X__ -D__EPOC32__ -D__MARM__ -D__EABI__ -D__MARM_ARMV5__ -D__SUPPORT_CPP_EXCEPTIONS__ -D__GCCE__ -DUNICODE -D_STLP_NO_EXCEPTION_HEADER -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -D__EXE__ FBReader_reg.rss -o ./FBReader_reg.rpp && rcomp -u -m045,046,047 -s./FBReader_reg.rpp -o./FBReader_reg.rsc -h./FBReader_reg.rsg -iFBReader_reg.rss

	mv fbreader/FBReader.exe ./

	/usr/share/qt4/qt-symbian/bin/createpackage  FBReader_template.pkg
	/usr/share/qt4/qt-symbian/bin/createpackage  FBReader_installer.pkg
