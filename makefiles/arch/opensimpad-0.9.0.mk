include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR=/opt/QtPalmtop
IMAGEDIR = $(INSTALLDIR)/pics
APPIMAGEDIR = $(IMAGEDIR)/%application_name%

BASEPATH = /stuff/build/opensimpad-0.9.0

TOOLSPATH = $(BASEPATH)/cross/arm-linux/bin
OEPATH = $(BASEPATH)/staging/arm-linux
QTPATH = $(BASEPATH)/staging/arm-linux/qt2

MOC = $(BASEPATH)/staging/i686-linux/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTPATH)/include
UILIBS = -L $(QTPATH)/lib -lqte -L $(OEPATH)/lib -lqpe -ljpeg -lpng12 -lz -lts
CC = $(TOOLSPATH)/gcc -I $(OEPATH)/include
AR = $(TOOLSPATH)/ar rsu
LD = $(TOOLSPATH)/g++

XML_LIB = -L $(OEPATH)/lib -lexpat
ARCHIVER_LIB = -L $(OEPATH)/lib -lz -lbz2

CFLAGS = -pipe -DOPIE_NO_DEBUG -DQT_NO_DEBUG -DQWS -fno-exceptions -fno-rtti -march=armv4 -mtune=xscale -mfloat-abi=soft --param large-function-growth=2000 --param inline-unit-growth=200 -Wall -Wno-ctor-dtor-privacy -W -Winline
LDFLAGS = -Wl,-rpath,$(LIBDIR)
