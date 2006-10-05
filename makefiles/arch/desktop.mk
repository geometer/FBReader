ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr/local
endif
ifeq "$(BINDIR)" ""
  BINDIR=$(INSTALLDIR)/bin
endif
ifeq "$(LIBDIR)" ""
  LIBDIR=$(INSTALLDIR)/lib
endif
ifeq "$(SHAREDIR)" ""
  SHAREDIR=$(INSTALLDIR)/share
endif

CC = gcc
AR = ar rsu
LD = g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DINSTALLDIR=\"$(INSTALLDIR)\" -DLIBICONV_PLUG
LDFLAGS = -Wl,-rpath,$(LIBDIR)

MOC = moc-qt3
QTINCLUDE = -I /usr/include/qt3

GTKINCLUDE = $(shell pkg-config --cflags gtk+-2.0 libpng xft)

ifeq "$(UI_TYPE)" "qt"
	UILIBS = -lqt-mt
else
	UILIBS = $(shell pkg-config --libs gtk+-2.0) -lpng -ljpeg
endif

EXPATLIBS = -lexpat
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

RM = rm -rvf
RM_QUIET = rm -rf
