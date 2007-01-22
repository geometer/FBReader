# ifeq "$(INSTALLDIR)" ""
#   INSTALLDIR=/usr/local
# endif
# IMAGEDIR = $(INSTALLDIR)/share/%APPLICATION_NAME%/icons
# 
BUILD_SHARED_LIBRARY = no

CC = i586-mingw32msvc-gcc
AR = i586-mingw32msvc-ar rsu
LD = i586-mingw32msvc-g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -I /home/geometer/win32/my/include -D_WIN32_IE=0x0600
LDFLAGS = -mwindows -L /home/geometer/win32/my/lib
# LDFLAGS = -Wl,-rpath,$(LIBDIR)

UILIBS = -lgdi32 -lcomctl32
EXPATLIBS = -lexpat -lz -liconv
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

RM = rm -rvf
RM_QUIET = rm -rf
