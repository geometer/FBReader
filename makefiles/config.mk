include $(ROOTDIR)/makefiles/platforms.mk

MAKE = make ROOTDIR=$(ROOTDIR)
LIBMAKE = $(MAKE) BUILD_SHARED_LIBRARY=yes

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

CFLAGS += -DINSTALLDIR=\"$(INSTALLDIR)\" -DBASEDIR=\"$(BASEDIR)\" -DHOMEDIR=\"$(HOMEDIR)\"

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

ZINCLUDE = -I $(ROOTDIR)/zlibrary/include -I $(ROOTDIR)/zldictionary/include
