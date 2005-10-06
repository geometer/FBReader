include $(ROOTDIR)/makefiles/platforms.mk

DUMMY_SECTIONS = \
	-DXML_SECTION="" \
	-DFS_SECTION="" \
	-DOPTIONS_SECTION="" \
	-DUTIL_SECTION="" \
	-DDIALOG_SECTION="" \
	-DIMAGE_SECTION="" \
	-DAPPLICATION_SECTION="" \
	-DMODEL_SECTION="" \
	-DVIEW_SECTION="" \
	-DVIEW1_SECTION="" \
	-DZLVIEW_SECTION="" \
	-DFB_VIEW_SECTION="" \
	-DTEXT_STYLE_SECTION="" \
	-DTEXT_STYLE1_SECTION="" \
	-DHYPHENATION_SECTION="" \
	-DFORMATS_SECTION="" \
	-DFB_SECTION="" \
	-DFB_DIALOG_SECTION=""

MAKE = ROOTDIR=$(ROOTDIR) make

ifeq "$(TARGET_ARCH)" "zaurus-qtopia"
	QTDIR = /opt/Qtopia/sharp
	TOOLSDIR = /opt/Embedix/tools/arm-linux/bin
	MOC = $(QTDIR)/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I $(QTDIR)/include
	QTLIBS = -L $(QTDIR)/lib -lqte -lqpe
	CC = $(TOOLSDIR)/gcc
	AR = $(TOOLSDIR)/ar rsu
	LD = $(TOOLSDIR)/g++

	CFLAGS = -pipe $(DUMMY_SECTIONS) -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
	DEPGEN = $(CC) -MM

	EXTERNALINCLUDE = -I $(ROOTDIR)/external/include
	EXPATLIBS = -L $(ROOTDIR)/external/lib -lexpat
	ENCALIBS = -L $(ROOTDIR)/external/lib -lenca
endif

ifeq "$(TARGET_ARCH)" "qvfb-opie"
	INSTALLDIR = /usr/local

	QTDIR = /opt/opie/qt-2.3.10
	OPIEDIR = /opt/opie/opie
	MOC = $(QTDIR)/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I $(QTDIR)/include -I $(OPIEDIR)/include
	QTLIBS = -L $(QTDIR)/lib -lqte -L $(OPIEDIR)/lib -lqpe -lexpat -lenca
	CC = gcc
	AR = ar rsu
	LD = g++

	CFLAGS = -pipe -DOPIE_NO_DEBUG -DOPIE_NO_BUILTIN_SHUTDOWN -DUSE_FILE_NOTIFICATION -DOPIE_SOUND_FRAGMENT_SHIFT=16 -DQT_NO_DEBUG $(DUMMY_SECTIONS) -DINSTALLDIR=\"$(INSTALLDIR)\" -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
	DEPGEN = $(CC) -MM
endif

ifeq "$(TARGET_ARCH)" "opensimpad-opie"
	TOOLSDIR = /stuff/build/tmp/cross/arm-linux/bin
	OEDIR = /stuff/build/tmp/staging/arm-linux
	QTDIR = /stuff/build/tmp/staging/arm-linux/qt2

	MOC = /stuff/build/tmp/staging/i686-linux/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I $(QTDIR)/include
	QTLIBS = -L $(QTDIR)/lib -lqte -L $(OEDIR)/lib -lqpe -lexpat -lenca -ljpeg -lpng12 -lz -lts
	CC = $(TOOLSDIR)/gcc -I $(OEDIR)/include
	AR = $(TOOLSDIR)/ar rsu
	LD = $(TOOLSDIR)/g++

	CFLAGS = -pipe -DOPIE_NO_DEBUG -DQT_NO_DEBUG $(DUMMY_SECTIONS) -DINSTALLDIR=\"$(INSTALLDIR)\" -DQWS -fno-exceptions -fno-rtti -march=armv4 -mtune=xscale -mfloat-abi=soft --param large-function-growth=2000 --param inline-unit-growth=200 -Wall -Wno-ctor-dtor-privacy -W -Winline
	DEPGEN = $(CC) -MM
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-qt"
ifeq "$(PDAXROM_VERSION)" "rc11"
	BASEDIR = /opt/cross/arm/3.4.4-xscale-softvfp
else
	BASEDIR = /opt/arm/3.3.2-vfp
endif
	CC = $(BASEDIR)/bin/armv5tel-linux-gcc -I $(BASEDIR)/include
	AR = $(BASEDIR)/bin/armv5tel-linux-ar rsu
	LD = $(BASEDIR)/bin/armv5tel-linux-g++

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe $(DUMMY_SECTIONS) -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
	MOC = $(BASEDIR)/bin/moc
	RM = rm -rvf
	RM_QUIET = rm -rf
	USRDIR = $(BASEDIR)/armv5tel-cacko-linux
	QTINCLUDE = -I $(USRDIR)/qt/include
	QTLIBS = -L $(USRDIR)/qt/lib -lqt-mt -L $(USRDIR)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
	EXPATLIBS = -lexpat
	#ENCALIBS = -L $(USRDIR)/lib -lenca
	ENCALIBS = $(USRDIR)/lib/libenca.so.0
endif

ifeq "$(TARGET_ARCH)" "zaurus-pdaxrom-gtk"
	BASEDIR = /opt/arm/3.3.2-vfp
	CC = $(BASEDIR)/bin/armv5tel-linux-gcc
	AR = $(BASEDIR)/bin/armv5tel-linux-ar rsu
	LD = $(BASEDIR)/bin/armv5tel-linux-g++

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe $(DUMMY_SECTIONS) -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
	RM = rm -rvf
	RM_QUIET = rm -rf
	USRDIR = $(BASEDIR)/armv5tel-cacko-linux
	INCDIR = $(USRDIR)/include
	GTKINCLUDE = -I$(INCDIR)/libxml2 -I$(INCDIR)/libglade-2.0 -I$(INCDIR)/gtk-2.0 -I$(USRDIR)/lib/gtk-2.0/include -I$(USRDIR)/X11R6/include -I$(INCDIR)/atk-1.0 -I$(INCDIR)/pango-1.0 -I$(INCDIR)/freetype2 -I$(INCDIR)/glib-2.0 -I$(USRDIR)/lib/glib-2.0/include
	GTKLIBS = -L $(USRDIR)/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lm -lgmodule-2.0 -ldl -lgthread-2.0 -L $(USRDIR)/X11R6/lib -lXext -lXrender -lXrandr -lXcursor -lX11 -lXft -lSM -lICE
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ifeq "$(TARGET_ARCH)" "desktop-qt"
	INSTALLDIR=/usr/local

	CC = gcc
	AR = ar rsu
	LD = g++

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe $(DUMMY_SECTIONS) -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DINSTALLDIR=\"$(INSTALLDIR)\"
	MOC = moc-qt3
	RM = rm -rvf
	RM_QUIET = rm -rf
	QTINCLUDE = -I /usr/include/qt3
	QTLIBS = -lqt-mt
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ifeq "$(TARGET_ARCH)" "desktop-gtk"
	INSTALLDIR=/usr/local

	CC = gcc
	AR = ar rsu
	LD = g++

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe $(DUMMY_SECTIONS) -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DINSTALLDIR=\"$(INSTALLDIR)\"
	RM = rm -rvf
	RM_QUIET = rm -rf
	GTKINCLUDE := $(shell pkg-config --cflags gtk+-2.0)
	GTKLIBS := $(shell pkg-config --libs gtk+-2.0)
	EXPATLIBS = -lexpat
	ENCALIBS = -lenca
endif

ifeq "$(TARGET_ARCH)" "maemo"
	INSTALLDIR=/usr

	CC = gcc
	AR = ar rsu
	LD = g++

	DEPGEN = $(CC) -MM
	CFLAGS = -pipe $(DUMMY_SECTIONS) -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
	RM = rm -rvf
	RM_QUIET = rm -rf
	GTKINCLUDE = -I/usr/include/libxml2 -I/usr/include/libglade-2.0 -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/X11R6/include -I/usr/include/atk-1.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/hildon-lgpl -I/usr/include/dbus-1.0
	GTKLIBS = -lz -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lm -lgmodule-2.0 -ldl -lgthread-2.0 -lhildonlgpl -lhildonwidgets -losso
	EXPATLIBS = -lexpat
	ENCALIBS = /usr/lib/libenca.a
endif

ifeq "$(TARGET_ARCH)" "palm"
	CC = m68k-palmos-g++
	AR = m68k-palmos-ar rsu
	LD = m68k-palmos-g++

	DEPGEN = $(CC) -MM -DPALM_TEMPORARY
	CFLAGS = \
		-DSTL_SECTION="__attribute__ ((section(\"stl0\")))" \
		-DSTL1_SECTION="__attribute__ ((section(\"stl1\")))" \
		-DSTL2_SECTION="__attribute__ ((section(\"stl1\")))" \
		-DXML_SECTION="__attribute__ ((section(\"zlb2\")))" \
		-DFS_SECTION="__attribute__ ((section(\"zlb2\")))" \
		-DOPTIONS_SECTION="__attribute__ ((section(\"zlb1\")))" \
		-DUTIL_SECTION="__attribute__ ((section(\"zlb0\")))" \
		-DDIALOG_SECTION="__attribute__ ((section(\"zlb1\")))" \
		-DIMAGE_SECTION="__attribute__ ((section(\"zlb1\")))" \
		-DAPPLICATION_SECTION="__attribute__ ((section(\"zlb0\")))" \
		-DZLVIEW_SECTION="__attribute__ ((section(\"zlb0\")))" \
		-DVIEW_SECTION="__attribute__ ((section(\"app0\")))" \
		-DVIEW1_SECTION="__attribute__ ((section(\"app1\")))" \
		-DMODEL_SECTION="__attribute__ ((section(\"app3\")))" \
		-DFB_VIEW_SECTION="__attribute__ ((section(\"app1\")))" \
		-DFB_SECTION="__attribute__ ((section(\"app1\")))" \
		-DTEXT_STYLE_SECTION="__attribute__ ((section(\"app0\")))" \
		-DTEXT_STYLE1_SECTION="__attribute__ ((section(\"app1\")))" \
		-DHYPHENATION_SECTION="__attribute__ ((section(\"app2\")))" \
		-DFORMATS_SECTION="__attribute__ ((section(\"app2\")))" \
		-DFB_DIALOG_SECTION="__attribute__ ((section(\"app4\")))" \
		-DPALM_TEMPORARY \
		-DUSE_OWN_XML_PARSER \
		-pipe -fno-exceptions -fno-rtti -fno-inline -Wall -Wno-ctor-dtor-privacy -W -Wno-non-template-friend
	RM = rm -rvf
	RM_QUIET = rm -rf
 	EXTERNALINCLUDE = -I $(ROOTDIR)/zlibrary/palm/pseudostl -I $(ROOTDIR)/palm-external
 	EXTERNALLIBS =
endif

ifeq "$(TARGET_STATUS)" "release"
	CFLAGS += -O3
	LDFLAGS += -s
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
endif

ZDIR = $(ROOTDIR)/zlibrary
ZINCLUDE = -I $(ROOTDIR)/zlibrary/include
