ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = -lm -L$(ROOTDIR)/zlibrary -lzlibrary-$(UI_TYPE) $(ENCALIBS) $(EXPATLIBS) $(BZIP2LIBS) $(UILIBS)

TARGET = FBReader

ALL_SUBDIRS = src src/model src/description src/collection src/bookmodel src/formats src/formats/fb2 src/formats/docbook src/formats/html src/formats/pdb src/formats/txt src/formats/tcr src/formats/chm src/formats/oeb src/formats/rtf src/formats/dummy src/formats/util src/textview src/hyphenation src/dictionary src/optionsDialog src/fbreader desktop pdaxrom
ALL_ARCHSUBDIRS = desktop pdaxrom opie zaurus maemo openzaurus pma400 win32

SUBDIRS = src src/model src/description src/collection src/bookmodel src/formats src/formats/fb2 src/formats/html src/formats/pdb src/formats/txt src/formats/tcr src/formats/chm src/formats/oeb src/formats/rtf src/formats/util src/textview src/hyphenation src/dictionary src/optionsDialog src/fbreader

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` $(LIBS)
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
