include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /mmc/mmca1/.system/QTDownLoad/fbreader
IMAGEDIR = $(INSTALLDIR)/pics
APPIMAGEDIR = $(IMAGEDIR)/%APPLICATION_NAME%

ARM_PATH = /usr/local/a1200/gcc-arm-iwmmxt/gcc-3.3.6-glibc-2.3.2/arm-linux
TOOLS_PATH = $(ARM_PATH)/arm-linux/bin
QT_PATH = $(ARM_PATH)/qt

CC = $(TOOLS_PATH)/gcc
AR = $(TOOLS_PATH)/ar rsu
LD = $(TOOLS_PATH)/g++
MOC = $(QT_PATH)/bin/moc

RM = rm -rvf
RM_QUIET = rm -rf

QTINCLUDE = -I $(QT_PATH)/include -I $(QT_PATH)/include/ezx
UILIBS = -L$(QT_PATH)/lib -L$(QT_PATH)/lib/ezx -lqte-mt -lezxappbase -lezxappsdk -lezxopenwindow -lipp-miscGen -lipp-jp -lezxjpeg -lezxpm

CFLAGS = -pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -mcpu=iwmmxt -mtune=iwmmxt -Wno-ctor-dtor-privacy -W -ftemplate-depth-30
LDFLAGS = 

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2
