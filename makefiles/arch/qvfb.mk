include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /opt/opie/opie
IMAGEDIR = $(INSTALLDIR)/pics
APPIMAGEDIR = $(IMAGEDIR)/%application_name%

QTPATH = /opt/opie/qt-2.3.10
OPIEPATH = /opt/opie/opie
MOC = $(QTPATH)/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTPATH)/include -I $(OPIEPATH)/include
UILIBS = -L $(QTPATH)/lib -lqte -L $(OPIEPATH)/lib -lqpe
CC = gcc
AR = ar rsu
LD = g++

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

CFLAGS = -pipe -DOPIE_NO_DEBUG -DOPIE_NO_BUILTIN_SHUTDOWN -DUSE_FILE_NOTIFICATION -DOPIE_SOUND_FRAGMENT_SHIFT=16 -DQT_NO_DEBUG -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
DEPGEN = $(CC) -MM
