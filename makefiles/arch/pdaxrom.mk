USRDIR = $(BASEDIR)/armv5tel-cacko-linux
CC = $(BASEDIR)/bin/armv5tel-linux-gcc -I $(BASEDIR)/include
AR = $(BASEDIR)/bin/armv5tel-linux-ar rsu
LD = $(BASEDIR)/bin/armv5tel-linux-g++

DEPGEN = $(CC) -MM
CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG

MOC = $(BASEDIR)/bin/moc
QTINCLUDE = -I $(USRDIR)/qt/include

INCDIR = $(USRDIR)/include
GTKINCLUDE = -I$(INCDIR)/libxml2 -I$(INCDIR)/libglade-2.0 -I$(INCDIR)/gtk-2.0 -I$(USRDIR)/lib/gtk-2.0/include -I$(USRDIR)/X11R6/include -I$(INCDIR)/atk-1.0 -I$(INCDIR)/pango-1.0 -I$(INCDIR)/freetype2 -I$(INCDIR)/glib-2.0 -I$(USRDIR)/lib/glib-2.0/include

EXPATLIBS = -lexpat
ENCALIBS = -L $(USRDIR)/lib -lenca
#ENCALIBS = $(USRDIR)/lib/libenca.so.0
BZIP2LIBS = -lbz2

ifeq "$(UI_TYPE)" "qt"
	UILIBS = -L $(USRDIR)/qt/lib -lqt-mt -L $(USRDIR)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
else
	UILIBS = -L $(USRDIR)/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lm -lgmodule-2.0 -ldl -lgthread-2.0 -L $(USRDIR)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
endif

RM = rm -rvf
RM_QUIET = rm -rf
