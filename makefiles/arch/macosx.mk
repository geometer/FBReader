include $(ROOTDIR)/makefiles/arch/unix.mk

BINDIR = $(INSTALLDIR)/Contents/MacOS
SHAREDIR = $(INSTALLDIR)/Contents/Resources
IMAGEDIR = $(SHAREDIR)/icons
APPIMAGEDIR = $(IMAGEDIR)

# ~~ is a macros that will be expanded at runtime into QApplication::applicationDirPath() + "/.."
SHAREDIR_MACRO = ~~/Resources
IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
APPIMAGEDIR_MACRO = $(IMAGEDIR_MACRO)
QT_BASE_DIR = /Users/geometer/Qt5.0.0/5.0.0/clang_64

ZLSHARED = no

CCACHE = $(shell if which ccache > /dev/null; then echo "ccache"; fi) #if ccache is not installed, do not use it
CC = $(CCACHE) clang
AR = ar rsu
#LD = g++ -headerpad_max_install_names
LD = clang

ARCH_FLAGS = -arch x86_64 -mmacosx-version-min=10.5
CFLAGS = $(ARCH_FLAGS) -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -Qunused-arguments
LDFLAGS = $(ARCH_FLAGS)

EXTERNAL_LIBS = -liconv

ifeq "$(UI_TYPE)" "qt4"
  QTINCLUDE = -I $(QT_BASE_DIR)/include
  MOC = $(QT_BASE_DIR)/bin/moc
  UILIBS = # -framework QtCore -framework QtGui -framework QtNetwork
endif

RM = rm -rvf
RM_QUIET = rm -rf
