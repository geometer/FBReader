ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = $(UILIBS) -lm -L$(ROOTDIR)/zlibrary -lzlibrary-$(UI_TYPE) $(ENCALIBS) $(EXPATLIBS) $(BZIP2LIBS)

TARGET = FBReader

ALL_SUBDIRS = common common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/pdb common/formats/txt common/formats/tcr common/formats/chm common/formats/oeb common/formats/rtf common/formats/dummy common/formats/util common/textview common/hyphenation common/dictionary common/fbreader desktop pdaxrom
ALL_ARCHSUBDIRS = desktop pdaxrom opie zaurus maemo openzaurus pma400

SUBDIRS = common common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/html common/formats/pdb common/formats/txt common/formats/tcr common/formats/chm common/formats/oeb common/formats/rtf common/formats/util common/textview common/hyphenation common/dictionary common/fbreader

all:
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	$(LD) $(LDFLAGS) -o $(TARGET) `find common -name *.o` $(LIBS)
	@echo " OK"

do_install:
	@install -d $(DESTDIR)$(BINDIR)
	@install $(TARGET) $(DESTDIR)$(BINDIR)
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader
	@install -m 0644 $(wildcard share/FBReader/hyphenationPatterns.zip) $(DESTDIR)$(SHAREDIR)/FBReader
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/formats/html
	@install -m 0644 share/FBReader/formats/html/html.ent $(DESTDIR)$(SHAREDIR)/FBReader/formats/html
	@install -d $(DESTDIR)$(SHAREDIR)/FBReader/formats/xhtml
	@install -m 0644 $(wildcard share/FBReader/formats/xhtml/*.ent) $(DESTDIR)$(SHAREDIR)/FBReader/formats/xhtml
	@cd $(TARGET_ARCH); make install 

package:
	@cd $(TARGET_ARCH); make all 

clean:
	@for subdir in $(ALL_SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for subdir in $(ALL_ARCHSUBDIRS); do \
		cd $$subdir; make clean; cd ..; \
	done;
	@$(RM) $(TARGET) err
