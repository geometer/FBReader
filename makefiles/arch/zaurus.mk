QTDIR = /opt/Qtopia/sharp
TOOLSDIR = /opt/Embedix/tools/arm-linux/bin
MOC = $(QTDIR)/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTDIR)/include
UILIBS = -L $(QTDIR)/lib -lqte -lqpe
CC = $(TOOLSDIR)/gcc
AR = $(TOOLSDIR)/ar rsu
LD = $(TOOLSDIR)/g++

CFLAGS = -pipe -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DUSE_ZAURUS_FONTHACK
LDFLAGS = -Wl,-rpath,/opt/QtPalmtop/lib,-rpath,/usr/lib

EXPATLIBS = -lexpat
ENCALIBS = -lenca
BZIP2LIBS = -lbz2
