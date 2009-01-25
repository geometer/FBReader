include $(ROOTDIR)/makefiles/arch/unix.mk

ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr
endif
IMAGEDIR = $(INSTALLDIR)/share/pixmaps
APPIMAGEDIR = $(INSTALLDIR)/share/pixmaps/%APPLICATION_NAME%

ZLSHARED = no

CC = gcc
AR = ar rsu
LD = g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
LDFLAGS =
EXTERNAL_INCLUDE = -I/sw/include -I/opt/local/include
EXTERNAL_LIBS = -L/sw/lib -L/opt/local/lib

RM = rm -rvf
RM_QUIET = rm -rf
