BASEDIR = /opt/openzaurus-3.5.4/usr/local/arm/oe
TOOLSDIR = $(BASEDIR)/bin
INCDIR = $(BASEDIR)/arm-linux/include
LIBDIR = $(BASEDIR)/arm-linux/lib

RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(INCDIR)/qt2
GTKINCLUDE = -I$(LIBDIR)/glib-2.0/include -I$(LIBDIR)/gtk-2.0/include -I$(INCDIR)/glib-2.0 -I$(INCDIR)/gtk-2.0 -I$(INCDIR)/pango-1.0 -I$(INCDIR)/atk-1.0
MOC = $(TOOLSDIR)/moc
CC = $(TOOLSDIR)/arm-linux-gcc
AR = $(TOOLSDIR)/arm-linux-ar rsu
LD = $(TOOLSDIR)/arm-linux-g++

EXPATLIBS = -lexpat
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

ifeq "$(UI_TYPE)" "opie"
	UILIBS = -lqte -lqpe
else
	UILIBS = -lgpewidget -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0
endif

CFLAGS = -pipe -DOPIE_NO_DEBUG -DQT_NO_DEBUG -DINSTALLDIR=\"$(INSTALLDIR)\" -DQWS -fno-exceptions -fno-rtti -march=armv4 -mtune=xscale --param large-function-growth=2000 --param inline-unit-growth=200 -Wall -Wno-ctor-dtor-privacy -W -Winline
LDFLAGS = -Wl,-rpath,/opt/QtPalmtop/lib
