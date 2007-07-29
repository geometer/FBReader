ROOTDIR = $(CURDIR)/..

MAKEFILESDIR = $(ROOTDIR)/makefiles

include $(MAKEFILESDIR)/config.mk

TARGET = FBDictionary

SUBDIRS = src/main

all: .resources
	@for subdir in $(SUBDIRS); do \
		if ! $(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk; then \
			exit 1; \
		fi; \
	done;
	@echo -n "Linking $(TARGET) ..."
	@$(LD) $(LDFLAGS) -o $(TARGET) `find src -name *.o` -L$(ROOTDIR)/zlibrary/text -lzltext $(CORE_LIBS)
	@echo " OK"

install: all do_install

FBDSHAREDIR = $(DESTDIR)$(SHAREDIR)/$(TARGET)

APPIMAGEDIR_REAL = $(subst %APPLICATION_NAME%,$(TARGET),$(APPIMAGEDIR))

do_install:
	@install -d $(DESTDIR)$(BINDIR)
	@install $(TARGET) $(DESTDIR)$(BINDIR)
	@install -d $(FBDSHAREDIR)/resources
	@install -m 0644 $(wildcard data/resources/*.xml) $(FBDSHAREDIR)/resources
	@install -d $(FBDSHAREDIR)/default
	@install -m 0644 data/styles.xml $(FBDSHAREDIR)/default/styles.xml
	@install -m 0644 data/keymap.xml $(FBDSHAREDIR)/default/keymap.xml
	@install -d $(DESTDIR)$(APPIMAGEDIR_REAL)
	@install -m 0644 $(wildcard icons/$(TARGET)/*) $(DESTDIR)$(APPIMAGEDIR_REAL)

clean:
	@for subdir in $(SUBDIRS); do \
		$(MAKE) -C $$subdir -f $(MAKEFILESDIR)/subdir.mk clean; \
	done;
	@for target in $(ARCHSUBDIRS); do \
		if [ -d $$target ]; then \
			$(MAKE) -C $$target clean; \
		fi; \
	done;
	@$(RM) $(TARGET) err
