include $(ROOTDIR)/makefiles/arch/dummy.mk

INSTALLDIR = /usr/local

QTDIR = /opt/opie/qt-2.3.10
OPIEDIR = /opt/opie/opie
MOC = $(QTDIR)/bin/moc
RM = rm -rvf
RM_QUIET = rm -rf
QTINCLUDE = -I $(QTDIR)/include -I $(OPIEDIR)/include
UILIBS = -L $(QTDIR)/lib -lqte -L $(OPIEDIR)/lib -lqpe -lexpat -lenca
CC = gcc
AR = ar rsu
LD = g++

CFLAGS = -pipe -DOPIE_NO_DEBUG -DOPIE_NO_BUILTIN_SHUTDOWN -DUSE_FILE_NOTIFICATION -DOPIE_SOUND_FRAGMENT_SHIFT=16 -DQT_NO_DEBUG $(DUMMY_SECTIONS) -DINSTALLDIR=\"$(INSTALLDIR)\" -DQWS -fno-exceptions -fno-rtti -Wall -Wno-ctor-dtor-privacy -W
DEPGEN = $(CC) -MM
