include $(ROOTDIR)/makefiles/arch/unix.mk

DESTDIR ?= /Applications

INSTALLDIR = /FBReader.app
BINDIR = $(INSTALLDIR)/Contents/MacOS
SHAREDIR = $(INSTALLDIR)/Contents/Share
IMAGEDIR = $(SHAREDIR)/icons
APPIMAGEDIR = $(SHAREDIR)/icons

SHAREDIR_MACRO = ~~/Contents/Share
IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
APPIMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons

ZLSHARED = no

TOOLSPATH = /Developer/usr/bin
CC = $(TOOLSPATH)/gcc-4.2
AR = $(TOOLSPATH)/ar rsu
LD = $(TOOLSPATH)/g++-4.2

MACOS_VERSION = 10.5

#ARCH_FLAGS = -arch x86_64 -arch i386 -arch ppc7400 -arch ppc64
ARCH_FLAGS = -arch i386 -arch ppc7400
CFLAGS_NOARCH = \
	-fmessage-length=0 -pipe -fpascal-strings -fasm-blocks \
	-mdynamic-no-pic -W -Wall \
	-isysroot /Developer/SDKs/MacOSX$(MACOS_VERSION).sdk \
	-fvisibility=hidden -fvisibility-inlines-hidden \
	-mmacosx-version-min=$(MACOS_VERSION) \
	-gdwarf-2
CFLAGS = $(ARCH_FLAGS) $(CFLAGS_NOARCH)
LDFLAGS = $(ARCH_FLAGS) \
	-isysroot /Developer/SDKs/MacOSX$(MACOS_VERSION).sdk \
	-mmacosx-version-min=$(MACOS_VERSION)
EXTERNAL_INCLUDE = -I/usr/local/include
EXTERNAL_LIBS = -liconv

UILIBS = -framework Cocoa $(ROOTDIR)/zlibrary/ui/src/cocoa/application/CocoaWindow.o $(ROOTDIR)/zlibrary/ui/src/cocoa/library/ZLCocoaAppDelegate.o

RM = rm -rvf
RM_QUIET = rm -rf
