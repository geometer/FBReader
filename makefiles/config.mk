include $(ROOTDIR)/makefiles/platforms.mk

MAKE = ROOTDIR=$(ROOTDIR) make

ifeq "$(TARGET_ARCH)" "zaurus-cacko"
	QTDIR=/opt/Qtopia/sharp
	TOOLSDIR = /opt/Embedix/tools/arm-linux/bin
	MOC = $(QTDIR)/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I $(QTDIR)/include
	QTLIBS = -L $(QTDIR)/lib -lqte -lqpe
	CC = $(TOOLSDIR)/gcc
	AR = $(TOOLSDIR)/ar rsu
	LD = $(TOOLSDIR)/g++
	STRIP = $(TOOLSDIR)/strip

	CFLAGS = -pipe -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -O2 -DNO_DEBUG
	DEPGEN = $(CC) -MM

	EXTERNALINCLUDE = -I $(ROOTDIR)/external/include
	EXPATLIBS = -L $(ROOTDIR)/external/lib -lexpat
	ENCALIBS = -L $(ROOTDIR)/external/lib -lenca
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom"
	BASEDIR = /opt/arm/3.3.2-vfp
	CC = $(BASEDIR)/bin/armv5tel-linux-gcc
	AR = $(BASEDIR)/bin/armv5tel-linux-ar rsu
	LD = $(BASEDIR)/bin/armv5tel-linux-g++
	STRIP = $(BASEDIR)/bin/armv5tel-linux-strip

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -O2 -DNO_DEBUG
	MOC = $(BASEDIR)/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I $(BASEDIR)/armv5tel-cacko-linux/qt/include
	QTLIBS = -L $(BASEDIR)/armv5tel-cacko-linux/qt/lib -lqt-mt -L $(BASEDIR)/armv5tel-cacko-linux/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	CC = gcc
	AR = ar rsu
	LD = g++
	STRIP = strip

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -O2 -DNO_DEBUG
	MOC = moc-qt3
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I /usr/include/qt3
	QTLIBS = -lqt-mt
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	CC = gcc
	AR = ar rsu
	LD = g++
	STRIP = strip

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -O2 -DNO_DEBUG
	RM = rm -rvf
	RM_QUIET = rm -rf
	GTKINCLUDE = -I/usr/include/libxml2 -I/usr/include/libglade-2.0 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/X11R6/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gconf/2 -I/usr/include/orbit-2.0
	GTKLIBS = -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lgconf-2 -lORBit-2 -lm -lgmodule-2.0 -ldl -lgthread-2.0
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ZDIR = $(ROOTDIR)/zlibrary
ZINCLUDE = -I $(ROOTDIR)/zlibrary/include
