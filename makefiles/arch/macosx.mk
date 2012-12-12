include $(ROOTDIR)/makefiles/arch/unix.mk

BINDIR = $(INSTALLDIR)/Contents/MacOS
SHAREDIR = $(INSTALLDIR)/Contents/Resources
IMAGEDIR = $(SHAREDIR)/icons
APPIMAGEDIR = $(IMAGEDIR)

# ~~ is a macros that will be expanded at runtime into QApplication::applicationDirPath() + "/.."
SHAREDIR_MACRO = ~~/Resources
IMAGEDIR_MACRO = $(SHAREDIR_MACRO)/icons
APPIMAGEDIR_MACRO = $(IMAGEDIR_MACRO)

ZLSHARED = no

CCACHE = $(shell if which ccache > /dev/null; then echo "ccache"; fi) #if ccache is not installed, do not use it
CC = $(CCACHE) gcc
AR = ar rsu
LD = g++ -headerpad_max_install_names

ARCH_FLAGS = -arch x86_64 -mmacosx-version-min=10.5
CFLAGS = $(ARCH_FLAGS) -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W
LDFLAGS = $(ARCH_FLAGS)

EXTERNAL_LIBS = -liconv

ifeq "$(UI_TYPE)" "qt4"
  QTINCLUDE =
  MOC = moc
  UILIBS = -framework QtCore -framework QtGui -framework QtNetwork
endif

RM = rm -rvf
RM_QUIET = rm -rf
