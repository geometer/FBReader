include $(ROOTDIR)/makefiles/platforms.mk

MAKE = make ROOTDIR=$(ROOTDIR)
LIBMAKE = $(MAKE) BUILD_SHARED_LIBRARY=$(BUILD_SHARED_LIBRARY)

include $(ROOTDIR)/makefiles/arch/$(TARGET_ARCH).mk

ifeq "$(BINDIR)" ""
  BINDIR = $(INSTALLDIR)/bin
endif
ifeq "$(LIBDIR)" ""
  LIBDIR = $(INSTALLDIR)/lib
endif
ifeq "$(SHAREDIR)" ""
  SHAREDIR = $(INSTALLDIR)/share
endif
ifeq "$(BASEDIR)" ""
  BASEDIR = $(SHAREDIR)
endif
ifeq "$(HOMEDIR)" ""
  HOMEDIR = "~"
endif

CFLAGS += -DINSTALLDIR=\"$(INSTALLDIR)\" -DBASEDIR=\"$(BASEDIR)\" -DHOMEDIR=\"$(HOMEDIR)\" -DIMAGEDIR=\"$(IMAGEDIR)\" -DAPPIMAGEDIR=\"$(APPIMAGEDIR)\"
ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  CFLAGS += -fPIC
endif

ifeq "$(TARGET_STATUS)" "release"
	CFLAGS += -O3
	LDFLAGS += -s
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
endif

ZINCLUDE = -I $(ROOTDIR)/zlibrary/core/include -I $(ROOTDIR)/zlibrary/text/include

ifeq "$(BUILD_SHARED_LIBRARY)" ""
  BUILD_SHARED_LIBRARY = yes
endif

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/core -lzlcore $(XML_LIB) $(ARCHIVER_LIB) -ldl
else
  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/ui -lzlui-$(UI_TYPE) -L$(ROOTDIR)/zlibrary/core -lzlcore $(UILIBS) $(XML_LIB) $(ARCHIVER_LIB)
endif

ifneq "$(BUILD_RESOURCE_OBJECT)" "yes"
.resources:
endif
