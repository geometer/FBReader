ROOTDIR = $(PWD)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

LDFLAGS = -O2

LIBS = $(QTLIBS) $(GTKLIBS) -lm -L$(ZDIR) -lzlibrary $(EXPATLIBS) $(ENCALIBS)

TARGET = FBReader

SUBDIRS = common/model common/description common/collection common/bookmodel common/formats common/formats/fb2 common/formats/docbook common/formats/html common/formats/txt common/view common/textview common/hyphenation common/fbreader

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	GTKSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	QSUBDIRS = $(TARGET_ARCH)
endif

ifeq "$(TARGET_ARCH)" "palm"
	SUBDIRS = $(TARGET_ARCH)
endif

ALL_SUBDIRS = $(SUBDIRS) palm
ALL_QSUBDIRS = zaurus-cacko zaurus-pdaxrom desktop-qt
ALL_GTKSUBDIRS = desktop-gtk

all: .bin
ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	@$(MAKE) -C ipk-cacko
endif
ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom"
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
	@echo -n "Linking $@ ..."
	@$(LD) $(LDFLAGS) -o $@ $^ `find . -name *.o` $(LIBS)
	@echo " OK"
	@echo -n "Stripping $@ ..."
	@$(STRIP) $(TARGET)
	@echo " OK"

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
	@$(MAKE) -C ipk-cacko clean
	@$(MAKE) -C ipk-pdaxrom clean
	@$(MAKE) -C palm/prc clean
	@$(RM) $(TARGET) err
