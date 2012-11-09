include $(ROOTDIR)/makefiles/arch/unix.mk

BB10_SDK_BIN_FOLDER=/Applications/bbndk/host_10_0_9_52/darwin/x86/usr/bin

#BINDIR = $(INSTALLDIR)/Contents/MacOS
#SHAREDIR = $(INSTALLDIR)/Contents/Resources
#IMAGEDIR = $(SHAREDIR)/icons
#APPIMAGEDIR = $(IMAGEDIR)
#
## ~~ is a macros that will be expanded at runtime into QApplication::applicationDirPath() + "/.."
#SHAREDIR_MACRO = ~~/Resources
#IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
#APPIMAGEDIR_MACRO = $(IMAGEDIR_MACRO)
#
ZLSHARED = no

CCACHE = $(shell if which ccache > /dev/null; then echo "ccache"; fi) #if ccache is not installed, do not use it
CC = $(CCACHE) $(BB10_SDK_BIN_FOLDER)/qcc
AR = $(BB10_SDK_BIN_FOLDER)/ntoarmv7-ar rsu
LD = $(BB10_SDK_BIN_FOLDER)/qcc
MOC = $(BB10_SDK_BIN_FOLDER)/moc

ARCH_FLAGS = -Vgcc_ntoarmv7le
CFLAGS = $(ARCH_FLAGS) -Wall -Wno-ctor-dtor-privacy -W
LDFLAGS = $(ARCH_FLAGS)

EXTERNAL_LIBS = -lcpp-ne -liconv

QTINCLUDE = -I /Applications/bbndk/target_10_0_9_386/qnx6/usr/include/qt4
UILIBS = -L /Applications/bbndk/target_10_0_9_386/qnx6/armle-v7/usr/lib/qt4/lib -lQtCore -lQtGui -lQtNetwork

#ifeq "$(UI_TYPE)" "qt4"
#  QTBASEDIR = ~/QtSDK/Desktop/Qt/4.8.1/gcc
#  QTINCLUDE = -I $(QTBASEDIR)/include
#  MOC = $(QTBASEDIR)/bin/moc
#  UILIBS = -F $(QTBASEDIR)/lib -framework QtCore -framework QtGui -framework QtNetwork
#endif
#
RM = rm -rvf
RM_QUIET = rm -rf
