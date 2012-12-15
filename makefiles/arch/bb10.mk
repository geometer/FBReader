include $(ROOTDIR)/makefiles/arch/unix.mk

QNX_HOST = /Applications/bbndk/host_10_0_9_404/darwin/x86
QNX_TARGET = /Applications/bbndk/target_10_0_9_1673/qnx6
BB10_SDK_BIN_FOLDER = $(QNX_HOST)/usr/bin

#BINDIR = $(INSTALLDIR)/Contents/MacOS
#SHAREDIR = $(INSTALLDIR)/Contents/Resources
#IMAGEDIR = $(SHAREDIR)/icons
#APPIMAGEDIR = $(IMAGEDIR)
#
## ~~ is a macros that will be expanded at runtime into QApplication::applicationDirPath() + "/.."
SHAREDIR_MACRO = app/native/resources
#IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
#APPIMAGEDIR_MACRO = $(IMAGEDIR_MACRO)
#
ZLSHARED = no

CCACHE = $(shell if which ccache > /dev/null; then echo "ccache"; fi) #if ccache is not installed, do not use it
CC = QNX_HOST=$(QNX_HOST) QNX_TARGET=$(QNX_TARGET) $(CCACHE) $(BB10_SDK_BIN_FOLDER)/qcc
AR = $(BB10_SDK_BIN_FOLDER)/ntoarmv7-ar rsu
LD = QNX_HOST=$(QNX_HOST) QNX_TARGET=$(QNX_TARGET) $(BB10_SDK_BIN_FOLDER)/qcc
MOC = $(BB10_SDK_BIN_FOLDER)/moc

ARCH_FLAGS = -Vgcc_ntoarmv7le
CFLAGS = $(ARCH_FLAGS) -Wall -Wno-ctor-dtor-privacy -W
LDFLAGS = $(ARCH_FLAGS)

EXTERNAL_LIBS = -lcpp-ne -liconv

QTINCLUDE = -I $(QNX_TARGET)/usr/include/qt4
UILIBS = -L $(QNX_TARGET)/armle-v7/usr/lib/qt4/lib -lQtCore -lQtGui -lQtNetwork

#ifeq "$(UI_TYPE)" "qt4"
#  QTBASEDIR = ~/QtSDK/Desktop/Qt/4.8.1/gcc
#  QTINCLUDE = -I $(QTBASEDIR)/include
#  MOC = $(QTBASEDIR)/bin/moc
#  UILIBS = -F $(QTBASEDIR)/lib -framework QtCore -framework QtGui -framework QtNetwork
#endif
#
RM = rm -rvf
RM_QUIET = rm -rf
