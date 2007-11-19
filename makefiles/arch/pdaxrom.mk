include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /usr
IMAGEDIR = /usr/share/pixmaps
APPIMAGEDIR = $(IMAGEDIR)/%APPLICATION_NAME%

BASEPATH = /opt/cross/arm/3.4.4-xscale-softvfp
USRPATH = $(BASEPATH)/armv5tel-cacko-linux
CC = $(BASEPATH)/bin/armv5tel-linux-gcc -I $(BASEPATH)/include
AR = $(BASEPATH)/bin/armv5tel-linux-ar rsu
LD = $(BASEPATH)/bin/armv5tel-linux-g++

DEPGEN = $(CC) -MM
CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG

MOC = $(BASEPATH)/bin/moc
QTINCLUDE = -I $(USRPATH)/qt/include

INCPATH = $(USRPATH)/include
GTKINCLUDE = -I$(INCPATH)/libxml2 -I$(INCPATH)/libglade-2.0 -I$(INCPATH)/gtk-2.0 -I$(USRPATH)/lib/gtk-2.0/include -I$(USRPATH)/X11R6/include -I$(INCPATH)/atk-1.0 -I$(INCPATH)/pango-1.0 -I$(INCPATH)/freetype2 -I$(INCPATH)/glib-2.0 -I$(USRPATH)/lib/glib-2.0/include

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

ifeq "$(UI_TYPE)" "qt"
	UILIBS = -L $(USRPATH)/qt/lib -lqt-mt -L $(USRPATH)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
else
	UILIBS = -L $(USRPATH)/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lm -lgmodule-2.0 -ldl -lgthread-2.0 -L $(USRPATH)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
endif

RM = rm -rvf
RM_QUIET = rm -rf
