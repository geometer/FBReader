include $(ROOTDIR)/makefiles/platforms.mk

VERSION = $(shell cat $(ROOTDIR)/fbreader/VERSION)
MAKE = make ROOTDIR=$(ROOTDIR)
LIBMAKE = $(MAKE) ZLSHARED=$(ZLSHARED)

include $(ROOTDIR)/makefiles/arch/$(TARGET_ARCH).mk

BINDIR ?= $(INSTALLDIR)/bin
LIBDIR ?= $(INSTALLDIR)/lib
INCDIR ?= $(INSTALLDIR)/include
SHAREDIR ?= $(INSTALLDIR)/share

INSTALLDIR_MACRO ?= $(INSTALLDIR)
LIBDIR_MACRO ?= $(LIBDIR)
SHAREDIR_MACRO ?= $(SHAREDIR)
IMAGEDIR_MACRO ?= $(IMAGEDIR)
APPIMAGEDIR_MACRO ?= $(APPIMAGEDIR)

XML_LIBS ?= -lexpat
ARCHIVER_LIBS ?= -lz -lbz2

CFLAGS += -DINSTALLDIR=\"$(INSTALLDIR_MACRO)\" -DBASEDIR=\"$(SHAREDIR_MACRO)\" -DLIBDIR=\"$(LIBDIR_MACRO)\" -DIMAGEDIR=\"$(IMAGEDIR_MACRO)\" -DAPPIMAGEDIR=\"$(APPIMAGEDIR_MACRO)\" -DVERSION=\"$(VERSION)\"
ifeq "$(ZLSHARED)" "yes"
  CFLAGS += -fPIC -DZLSHARED
endif

ifeq "$(TARGET_STATUS)" "release"
	CFLAGS += -O3
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g -Wall
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
	LDFLAGS += -pg
endif

ZINCLUDE = -I $(ROOTDIR)/zlibrary/core/include -I $(ROOTDIR)/zlibrary/text/include

ZLSHARED ?= yes

ifeq "$(ZLSHARED)" "yes"
  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/core -lzlcore
	TEXT_LIBS = -L$(ROOTDIR)/zlibrary/text -lzltext
	ZLUI_LIB = -L$(ROOTDIR)/zlibrary/ui -lzlui
else
  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/ui -L$(ROOTDIR)/zlibrary/core -lzlcore -lzlui -lzlcore $(UILIBS) $(XML_LIBS) $(ARCHIVER_LIBS)
	TEXT_LIBS = -L$(ROOTDIR)/zlibrary/text -lzltext $(EXTERNAL_LIBS) -lunibreak -lfribidi
endif

ifneq "$(BUILD_RESOURCE_OBJECT)" "yes"
.resources:
endif
