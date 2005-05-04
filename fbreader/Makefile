ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LIBS = $(QTLIBS) $(GTKLIBS) -lm -L$(ZDIR) -lzlibrary $(EXPATLIBS) $(ENCALIBS) $(EXTERNALLIBS)

TARGET = FBReader

ALL_SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/txt common/view common/textview common/hyphenation common/fbreader palm
ALL_QSUBDIRS = zaurus-cacko zaurus-pdaxrom-qt desktop-qt
ALL_GTKSUBDIRS = zaurus-pdaxrom-gtk desktop-gtk

SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/txt common/view common/textview common/hyphenation common/fbreader

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-qt"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-gtk"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "palm"
	SUBDIRS = common/model common/view common/textview $(TARGET_ARCH)
	#SUBDIRS = common/model common/view $(TARGET_ARCH)
endif

all: .bin
ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	@$(MAKE) -C ipk-cacko
endif
ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-qt"
	@$(MAKE) -C ipk-pdaxrom
endif
ifeq "$(TARGET_ARCH)" "palm"
	@$(MAKE) -C palm/prc
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
		echo $$subdir; \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for subdir in $(ALL_QSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/qsubdir.mk clean; \
	done;
	@for subdir in $(ALL_GTKSUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/gtksubdir.mk clean; \
	done;
	@$(MAKE) -C ipk-cacko clean
	@$(MAKE) -C ipk-pdaxrom clean
	@$(MAKE) -C palm/prc clean
	@$(RM) $(TARGET) err
