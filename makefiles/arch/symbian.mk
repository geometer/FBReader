ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr
endif
IMAGEDIR = !!\\\\pixmaps
APPIMAGEDIR = !!\\\\pixmaps\\\\FBReader
EPOCROOT = /usr/s60-sdk

CC = arm-none-symbianelf-g++
AR = arm-none-symbianelf-ar cqs
LD = arm-none-symbianelf-ld


DEFINES       = -DUNICODE -DQT_KEYPAD_NAVIGATION -DQT_SOFTKEYS_ENABLED -DQT_USE_MATH_H_FLOATS -D__PRODUCT_INCLUDE__=\</usr/s60-sdk/epoc32/include/variant/symbian_os.hrh\> -D__SYMBIAN32__ -D__MARM_INTERWORK__ -D_UNICODE -D__S60_50__ -D__S60_3X__ -D__SERIES60_3X__ -D__EPOC32__ -D__MARM__ -D__EABI__ -D__MARM_ARMV5__ -D__SUPPORT_CPP_EXCEPTIONS__ -D__GCCE__ -DUNICODE -D_STLP_NO_EXCEPTION_HEADER -DQT_GUI_LIB -DQT_CORE_LIB

CXXFLAGS      = -pipe --include=${EPOCROOT}/epoc32/include/gcce/gcce.h -march=armv5t -mapcs -mthumb-interwork -nostdinc -c -msoft-float -T script -x c++ -fexceptions -fno-unit-at-a-time -fvisibility-inlines-hidden 

CXXFLAGS += -DLIBICONV_PLUG -DDO_ICONV_CAST -DFILENAMEDELIMETER=\"\\\" -DROOTPATH=\"\" -fvisibility-inlines-hidden

INCPATH       = -I/usr/share/qt4/qt-symbian/mkspecs/symbian/linux-gcce -I. -I/usr/share/qt4/qt-symbian/include -I${EPOCROOT}/epoc32/include/ -I/usr/s60-sdk/epoc32/include/variant -I/usr/s60-sdk/epoc32/include/stdapis -I/usr/s60-sdk/epoc32/include/gcce -I${EPOCROOT}/epoc32/include/stdapis/sys -I${EPOCROOT}/epoc32/include/stdapis/stlport -I/usr/s60-sdk/epoc32-s60/include -I/usr/s60-sdk/epoc32/include/oem -I/usr/s60-sdk/epoc32/include/middleware -I/usr/s60-sdk/epoc32/include/domain/middleware -I/usr/s60-sdk/epoc32/include/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc -I/usr/s60-sdk/epoc32/include/domain/osextensions/loc/sc -I/usr/s60-sdk/epoc32/include/domain/middleware/loc/sc -I.

#CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG
CFLAGS = $(DEFINES) $(CXXFLAGS) $(INCPATH) -w
LDFLAGS = --target1-abs --no-undefined --nostdlib \
-L/usr/arm-none-symbianelf/bin/../lib/gcc/arm-none-symbianelf/4.4.1/ -L/usr/arm-none-symbianelf/bin/../lib/gcc/ -L/usr/arm-none-symbianelf/bin/../lib/gcc/arm-none-symbianelf/4.4.1/../../../../arm-none-symbianelf/lib/arm-none-symbianelf/4.4.1/ -L/usr/arm-none-symbianelf/bin/../lib/gcc/arm-none-symbianelf/4.4.1/../../../../arm-none-symbianelf/lib/  \
-L/usr/s60-sdk/epoc32/release/armv5/udeb/ -L/usr/s60-sdk//epoc32/release/armv5/lib -L/usr/s60-sdk/epoc32/release/armv5/urel -L/usr/share/qt4/qt-symbian/lib -L/usr/s60-sdk/epoc32/release/armv5/udeb \
-Tdata 0x700000 -Ttext 0x80000 --shared --soname fbreader\{00040701\}\[e87cc83c\].exe --entry=_E32Startup -u _E32Startup 


# -Iinclude -I../text/include -I../../libs/symbian/include

MOC = moc-symbian
QTINCLUDE = -I/usr/share/qt4/qt-symbian/include/QtCore -I/usr/share/qt4/qt-symbian/include/QtGui
EXTERNAL_INCLUDE = -I$(ROOTDIR)/libs/symbian/include
ZLSHARED=no

ifeq "$(UI_TYPE)" "qt"
  UILIBS = -lqt-mt
endif

ifeq "$(UI_TYPE)" "qt4"
  UILIBS = -lQtGui
endif

ifeq "$(UI_TYPE)" "gtk"
  UILIBS = $(shell pkg-config --libs gtk+-2.0 gio-2.0) -lpng -ljpeg
endif

RM = rm -rvf
RM_QUIET = rm -rf
