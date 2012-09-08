include $(ROOTDIR)/makefiles/arch/unix.mk

BINDIR = $(INSTALLDIR)/Contents/MacOS
SHAREDIR = $(INSTALLDIR)/Contents/Resources
IMAGEDIR = $(SHAREDIR)/icons
APPIMAGEDIR = $(IMAGEDIR)

# ~~ is a macros that will be expanded at runtime into QApplication::applicationDirPath() + "/.."
SHAREDIR_MACRO = ~~/Resources
IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
APPIMAGEDIR_MACRO = $(IMAGEDIR_MACRO)

CCACHE = $(shell if which ccache > /dev/null; then echo "ccache"; fi) #if ccache is not installed, do not use it
CC = $(CCACHE) gcc
AR = ar rsu
LD = g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W
LDFLAGS =

ifeq "$(UI_TYPE)" "qt4"
  QTBASEDIR = ~/QtSDK/Desktop/Qt/4.8.1/gcc
  QTINCLUDE = -I $(QTBASEDIR)/include
  MOC = $(QTBASEDIR)/bin/moc
  UILIBS = -F $(QTBASEDIR)/lib -framework QtCore -framework QtGui -framework QtNetwork
endif

RM = rm -rvf
RM_QUIET = rm -rf
