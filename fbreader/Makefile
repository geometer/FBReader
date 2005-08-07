ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = $(QTLIBS) $(GTKLIBS) -lm -L$(ZDIR) -lzlibrary $(EXPATLIBS) $(ENCALIBS) $(EXTERNALLIBS)

TARGET = FBReader

ALL_SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/pdb common/formats/txt common/textview common/hyphenation common/fbreader palm
ALL_QSUBDIRS = zaurus-qtopia zaurus-pdaxrom-qt desktop-qt
ALL_GTKSUBDIRS = zaurus-pdaxrom-gtk desktop-gtk maemo

SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/html common/formats/pdb common/formats/txt common/textview common/hyphenation common/fbreader

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "maemo"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-qt"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-gtk"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-qtopia"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "palm"
	SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/textview common/hyphenation common/fbreader
	SUBDIRS += $(TARGET_ARCH)
endif

all: .bin
	@if [ -d $(TARGET_ARCH)/package ]; then \
		$(MAKE) -C $(TARGET_ARCH)/package; \
	fi;

install: all .really-install

.really-install:
ifeq "$(TARGET_ARCH)" "desktop-qt"
	install -d $(DESTDIR)$(INSTALLDIR)/bin
	install -s FBReader $(DESTDIR)$(INSTALLDIR)/bin
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -m 0644 share/FBReader/formats/html/html.ent $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -m 0644 icons/640x480/FBReader.png $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons
	install -m 0644 $(wildcard icons/640x480/FBReader/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/encodings/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/*.zip) $(DESTDIR)$(INSTALLDIR)/share/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	install -m 0644 $(wildcard share/FBReader/help/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
endif
ifeq "$(TARGET_ARCH)" "desktop-gtk"
	install -d $(DESTDIR)$(INSTALLDIR)/bin
	install -s FBReader $(DESTDIR)$(INSTALLDIR)/bin
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -m 0644 share/FBReader/formats/html/html.ent $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -m 0644 icons/640x480/FBReader.png $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons
	install -m 0644 $(wildcard icons/640x480/FBReader/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/encodings/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/*.zip) $(DESTDIR)$(INSTALLDIR)/share/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	install -m 0644 $(wildcard share/FBReader/help/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
endif
ifeq "$(TARGET_ARCH)" "maemo"
	install -d $(DESTDIR)$(INSTALLDIR)/bin
	install -s FBReader $(DESTDIR)$(INSTALLDIR)/bin
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -m 0644 share/FBReader/formats/html/html.ent $(DESTDIR)$(INSTALLDIR)/share/FBReader/formats/html
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -m 0644 $(wildcard icons/blueset/FBReader/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/icons/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/encodings/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/encodings
	install -m 0644 $(wildcard share/FBReader/*.zip) $(DESTDIR)$(INSTALLDIR)/share/FBReader
	install -d $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	install -m 0644 $(wildcard share/FBReader/help/*) $(DESTDIR)$(INSTALLDIR)/share/FBReader/help
	install -d $(DESTDIR)/usr/share/icons/hicolor/26x26
	install icons/blueset/FBReader.png $(DESTDIR)/usr/share/icons/hicolor/26x26/
	install -d $(DESTDIR)/usr/share/applications/hildon
	install -m 0644 maemo/package/FBReader.desktop $(DESTDIR)/usr/share/applications/hildon
	install -d $(DESTDIR)/etc/others-menu
	( cd $(DESTDIR)/etc/others-menu ; ln -sf ../../usr/share/applications/hildon/FBReader.desktop 9900_FBReader.desktop )
endif

.lib:
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@for subdir in $(QSUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk; then \
			exit 1; \
		fi; \
	done;
	@for subdir in $(GTKSUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk; then \
			exit 1; \
		fi; \
	done;

.bin: .lib
	@$(RM) $(TARGET)
	@$(MAKE) $(TARGET)

$(TARGET):
ifeq "$(TARGET_ARCH)" "palm"
	cd palm; \
	m68k-palmos-multigen $(TARGET).def; \
	m68k-palmos-as -o $(TARGET)-sections.o $(TARGET)-sections.s; \
	cd ..; \
	$(LD) $(LDFLAGS) -o $@ $^ `find . -name *.o` $(LIBS) -lPalmOSGlue palm/$(TARGET)-sections.ld
else
	@echo -n "Linking $@ ..."
	@$(LD) $(LDFLAGS) -o $@ $^ `find . -name *.o` $(LIBS)
	@echo " OK"
	@echo -n "Stripping $@ ..."
	@$(STRIP) $(TARGET)
	@echo " OK"
endif

clean:
	@for subdir in $(ALL_SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for subdir in $(ALL_QSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk clean; \
	done;
	@for subdir in $(ALL_GTKSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk clean; \
	done;
	@for target in $(ALL_TARGET_ARCHS); do \
		if [ -d $$target/package ]; then \
			$(MAKE) -C $$target/package clean; \
		fi; \
	done;
	@$(RM) $(TARGET) err
