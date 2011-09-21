# Meego 1.2 Harmattan
include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /usr
IMAGEDIR = $(INSTALLDIR)/share/%APPLICATION_NAME%/icons
APPIMAGEDIR = $(IMAGEDIR)

CC = gcc
AR = ar rsu

DEPGEN = $(CC) -MM
RM = rm -rvf
RM_QUIET = rm -rf

LD = g++
CFLAGS = -DMEEGO_VERSION_MAJOR=1 -DMEEGO_VERSION_MINOR=2 -DMEEGO_VERSION_PATCH=0 -DMEEGO_EDITION=harmattan -DMEEGO_EDITION_HARMATTAN
CFLAGS += -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
MOC = moc-qt4
QTINCLUDE = -I/usr/include/qt4 -I/usr/include/qt4/QtDeclarative -I/usr/include/qt4/QtScript -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtOpenGL -I/usr/include/resource/qt4
UILIBS = -lQtCore -lQtGui -lQtDeclarative -lQtOpenGL -lresourceqt
