include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /opt/QtPalmtop
ifeq "$(UI_TYPE)" "qtopia-240x320"
  IMAGEDIR = $(INSTALLDIR)/pics
else # UI_TYPE == qtopia-640x480
  IMAGEDIR = $(INSTALLDIR)/pics144
endif
APPIMAGEDIR = $(IMAGEDIR)/%APPLICATION_NAME%

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

CFLAGS = -pipe -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DUSE_ZAURUS_FONTHACK -ftemplate-depth-30
LDFLAGS = -Wl,-rpath,$(LIBDIR),-rpath,/usr/lib

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2
