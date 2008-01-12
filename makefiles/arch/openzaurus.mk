include $(ROOTDIR)/makefiles/arch/unix.mk

ifeq "$(UI_TYPE)" "opie"
  INSTALLDIR = /opt/QtPalmtop
  IMAGEDIR = $(INSTALLDIR)/pics
else # UI_TYPE == gpe
  INSTALLDIR = /usr
  IMAGEDIR = /usr/share/pixmaps
endif
APPIMAGEDIR = $(IMAGEDIR)/%application_name%

BASEPATH = /opt/openzaurus-3.5.4/usr/local/arm/oe
TOOLSPATH = $(BASEPATH)/bin
INCPATH = $(BASEPATH)/arm-linux/include
LIBPATH = $(BASEPATH)/arm-linux/lib

RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(INCPATH)/qt2
GTKINCLUDE = -I$(LIBPATH)/glib-2.0/include -I$(LIBPATH)/gtk-2.0/include -I$(INCPATH)/glib-2.0 -I$(INCPATH)/gtk-2.0 -I$(INCPATH)/pango-1.0 -I$(INCPATH)/atk-1.0
MOC = $(TOOLSPATH)/moc
CC = $(TOOLSPATH)/arm-linux-gcc
AR = $(TOOLSPATH)/arm-linux-ar rsu
LD = $(TOOLSPATH)/arm-linux-g++

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

ifeq "$(UI_TYPE)" "opie"
	UILIBS = -lqte -lqpe
else
	UILIBS = -lgpewidget -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0
endif

CFLAGS = -pipe -DOPIE_NO_DEBUG -DQT_NO_DEBUG -DQWS -fno-exceptions -fno-rtti -march=armv4 -mtune=xscale --param inline-unit-growth=200 --param large-function-growth=400 -Wall -Wno-ctor-dtor-privacy -W -Winline
LDFLAGS = -Wl,-rpath,$(LIBDIR)
