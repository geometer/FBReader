ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = -lm -L$(ROOTDIR)/zlibrary -lzlibrary-$(UI_TYPE) $(ENCA_LIB) $(UILIBS) $(XML_LIB) $(ARCHIVER_LIB)

TARGET = FBReader

ALL_SUBDIRS = src src/model src/description src/collection src/bookmodel src/formats src/formats/fb2 src/formats/docbook src/formats/html src/formats/pdb src/formats/txt src/formats/tcr src/formats/chm src/formats/oeb src/formats/rtf src/formats/openreader src/formats/dummy src/formats/util src/textview src/hyphenation src/external src/optionsDialog src/fbreader desktop pdaxrom
ALL_ARCHSUBDIRS = desktop pdaxrom opie zaurus maemo openzaurus pma400 win32

SUBDIRS = src src/model src/description src/collection src/bookmodel src/formats src/formats/fb2 src/formats/html src/formats/pdb src/formats/txt src/formats/tcr src/formats/chm src/formats/oeb src/formats/rtf src/formats/openreader src/formats/util src/textview src/hyphenation src/external src/optionsDialog src/fbreader

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` $(LIBS)
	@echo " OK"

FBSHAREDIR = $(DESTDIR)$(SHAREDIR)/FBReader
VARIANT = $(TARGET_ARCH)
ifneq "$(RESOLUTION)" "" 
  VARIANT = $(TARGET_ARCH)_$(RESOLUTION)
endif

do_install:
	@install -d $(DESTDIR)$(BINDIR)
	@install $(TARGET) $(DESTDIR)$(BINDIR)
	@install -d $(FBSHAREDIR)
	@install -m 0644 $(wildcard share/FBReader/hyphenationPatterns.zip) $(FBSHAREDIR)
	@install -d $(FBSHAREDIR)/help
	@install -m 0644 data/help/MiniHelp.$(VARIANT).fb2 $(FBSHAREDIR)/help/MiniHelp.fb2
	@install -d $(FBSHAREDIR)/formats/html
	@install -m 0644 share/FBReader/formats/html/html.ent $(FBSHAREDIR)/formats/html
	@install -d $(FBSHAREDIR)/formats/xhtml
	@install -m 0644 $(wildcard share/FBReader/formats/xhtml/*.ent) $(FBSHAREDIR)/formats/xhtml
	@install -d $(FBSHAREDIR)/default
	@install -m 0644 data/default/external.$(TARGET_ARCH).xml $(FBSHAREDIR)/default/external.xml
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
