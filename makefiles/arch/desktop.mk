include $(ROOTDIR)/makefiles/arch/unix.mk

ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr
endif
IMAGEDIR = $(INSTALLDIR)/share/pixmaps
APPIMAGEDIR = $(INSTALLDIR)/share/pixmaps/%APPLICATION_NAME%

CC = gcc
AR = ar rsu
LD = g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
LDFLAGS =

ifeq "$(UI_TYPE)" "qt"
  MOC = moc-qt3
  QTINCLUDE = -I /usr/include/qt3
else
  MOC = moc-qt4
  QTINCLUDE = -I /usr/include/qt4
endif

GTKINCLUDE = $(shell pkg-config --cflags gtk+-2.0 libpng xft)

ifeq "$(UI_TYPE)" "qt"
  UILIBS = -lqt-mt
endif

ifeq "$(UI_TYPE)" "qt4"
  UILIBS = -lQtGui
endif

ifeq "$(UI_TYPE)" "gtk"
  UILIBS = $(shell pkg-config --libs gtk+-2.0) -lpng -ljpeg
endif

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

RM = rm -rvf
RM_QUIET = rm -rf
