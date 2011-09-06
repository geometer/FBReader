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
NETWORK_LIBS ?= -lcurl

CFLAGS += -DINSTALLDIR=\"$(INSTALLDIR_MACRO)\" -DBASEDIR=\"$(SHAREDIR_MACRO)\" -DLIBDIR=\"$(LIBDIR_MACRO)\" -DIMAGEDIR=\"$(IMAGEDIR_MACRO)\" -DAPPIMAGEDIR=\"$(APPIMAGEDIR_MACRO)\" -DVERSION=\"$(VERSION)\"
ifeq "$(ZLSHARED)" "yes"
  CFLAGS += -fPIC -DZLSHARED
endif

ifeq "$(TARGET_STATUS)" "release"
#	CFLAGS += -O3
	LDFLAGS += -s
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
	LDFLAGS += -pg
endif

ZINCLUDE = -I $(ROOTDIR)/zlibrary/core/include -I $(ROOTDIR)/zlibrary/text/include

ZLSHARED ?= yes

ifeq "$(ZLSHARED)" "yes"
  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/core -lzlcore -ldl
	TEXT_LIBS = -lzltext
else
#  CORE_LIBS = -lm -L$(ROOTDIR)/zlibrary/ui -L$(ROOTDIR)/zlibrary/core -lzlcore -lzlui-$(UI_TYPE) -lzlcore $(UILIBS) $(XML_LIBS) $(ARCHIVER_LIBS) $(NETWORK_LIBS)
# TEXT_LIBS = -lzltext $(EXTERNAL_LIBS) -llinebreak -lfribidi

  TEXT_LIBS = -L$(ROOTDIR)/zlibrary/text -L$(ROOTDIR)/libs/symbian  -l:zltext.lib -l:linebreak.lib  -l:fribidi.lib -l:libm.dso

  CORE_LIBS = -L$(ROOTDIR)/zlibrary/core -L$(ROOTDIR)/zlibrary/ui -l:zlcore.lib -l:zlui.lib -l:zlcore.lib  -l:expat.lib -l:ezlib.dso -l:bz2.lib -l:sqlite3.dso \
-l:libc.dso -l:libdl.dso -l:euser.dso -l:eexe.lib -l:usrt2_2.lib -l:qtmain.lib -l:dfpaeabi.dso -l:drtaeabi.dso -l:scppnwdl.dso -lsupc++ -lgcc -l:remconcoreapi.dso -l:remconinterfacebase.dso -l:libstdcpp.dso -l:QtGui.dso -l:QtCore.dso -l:libpthread.dso 
endif

ifneq "$(BUILD_RESOURCE_OBJECT)" "yes"
.resources:
endif
