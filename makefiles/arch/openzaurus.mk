include $(ROOTDIR)/makefiles/arch/dummy.mk

BASEDIR = /stuff/build/openzaurus

TOOLSDIR = $(BASEDIR)/cross/arm-linux/bin
OEDIR = $(BASEDIR)/staging/arm-linux
QTDIR = $(BASEDIR)/staging/arm-linux/qt2

MOC = $(BASEDIR)/staging/i686-linux/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTDIR)/include
UILIBS = -L $(QTDIR)/lib -lqte -L $(OEDIR)/lib -lqpe -ljpeg -lpng12 -lz -lts
CC = $(TOOLSDIR)/gcc -I $(OEDIR)/include
AR = $(TOOLSDIR)/ar rsu
LD = $(TOOLSDIR)/g++

EXPATLIBS = -lexpat
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

CFLAGS = -pipe -DOPIE_NO_DEBUG -DQT_NO_DEBUG $(DUMMY_SECTIONS) -DINSTALLDIR=\"$(INSTALLDIR)\" -DQWS -fno-exceptions -fno-rtti -march=armv4 -mtune=xscale -mfloat-abi=soft --param large-function-growth=2000 --param inline-unit-growth=200 -Wall -Wno-ctor-dtor-privacy -W -Winline
DEPGEN = $(CC) -MM
