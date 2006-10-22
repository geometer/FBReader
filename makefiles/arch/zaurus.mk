INSTALLDIR = /opt/QtPalmtop

QTPATH = /opt/Qtopia/sharp
TOOLSPATH = /opt/Embedix/tools/arm-linux/bin
MOC = $(QTPATH)/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTPATH)/include
UILIBS = -L $(QTPATH)/lib -lqte -lqpe
CC = $(TOOLSPATH)/gcc
AR = $(TOOLSPATH)/ar rsu
LD = $(TOOLSPATH)/g++

CFLAGS = -pipe -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DUSE_ZAURUS_FONTHACK
LDFLAGS = -Wl,-rpath,$(LIBDIR),-rpath,/usr/lib

EXPATLIBS = -lexpat
ENCALIBS = -lenca
BZIP2LIBS = -lbz2
