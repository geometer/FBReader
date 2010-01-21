include $(ROOTDIR)/makefiles/arch/unix.mk

ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=$(ROOTDIR)/FBReader.app
endif
BINDIR=$(INSTALLDIR)/Contents/MacOS
SHAREDIR=$(INSTALLDIR)/Contents/Share
IMAGEDIR = $(SHAREDIR)/icons
APPIMAGEDIR = $(SHAREDIR)/icons/%APPLICATION_NAME%
BASEDIR = ~~/Contents/Share

ZLSHARED = no

TOOLS_DIR = /Developer/usr/bin
CC = $(TOOLS_DIR)/gcc-4.2
AR = $(TOOLS_DIR)/ar rsu
LD = $(TOOLS_DIR)/g++-4.2

ARCH = x86_64
MACOS_VERSION = 10.6

CFLAGS = \
	-arch $(ARCH) -fmessage-length=0 -pipe -fpascal-strings -fasm-blocks \
	-Os -mdynamic-no-pic -W -Wall \
	-isysroot /Developer/SDKs/MacOSX$(MACOS_VERSION).sdk \
	-fvisibility=hidden -fvisibility-inlines-hidden \
	-mmacosx-version-min=$(MACOS_VERSION) \
	-gdwarf-2
LDFLAGS = -arch $(ARCH) \
	-isysroot /Developer/SDKs/MacOSX$(MACOS_VERSION).sdk \
	-mmacosx-version-min=$(MACOS_VERSION)
EXTERNAL_INCLUDE = -I$(ROOTDIR)/3rdparty/include
EXTERNAL_LIBS = -liconv

UILIBS = -framework Cocoa $(ROOTDIR)/zlibrary/ui/src/cocoa/view/CocoaView.o $(ROOTDIR)/zlibrary/ui/src/cocoa/library/ZLCocoaAppDelegate.o

RM = rm -rvf
RM_QUIET = rm -rf
