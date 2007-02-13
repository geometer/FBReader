include $(ROOTDIR)/makefiles/platforms.mk

MAKE = make ROOTDIR=$(ROOTDIR)

include $(ROOTDIR)/makefiles/arch/$(TARGET_ARCH).mk

ifeq "$(BUILD_SHARED_LIBRARY)" ""
  BUILD_SHARED_LIBRARY = yes
endif
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

CFLAGS += -DINSTALLDIR=\"$(INSTALLDIR)\" -DBASEDIR=\"$(BASEDIR)\" -DHOMEDIR=\"$(HOMEDIR)\" -DIMAGEDIR=\"$(IMAGEDIR)\" -DAPPIMAGEDIR=\"$(APPIMAGEDIR)\" -DFILENAMEDELIMITER=\"$(FILENAMEDELIMITER)\" -DPATHDELIMITER=\"$(PATHDELIMITER)\"

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

ZINCLUDE = -I $(ROOTDIR)/zlibrary/include

ifneq "$(BUILD_RESOURCE_OBJECT)" "yes"
.resources:
endif
