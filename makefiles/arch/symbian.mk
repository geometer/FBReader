ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr
endif
SHAREDIR_MACRO=!!
IMAGEDIR = !!\\\\pixmaps
APPIMAGEDIR = !!\\\\pixmaps\\\\FBReader

EPOCROOT = /usr/s60-sdk
QTSYMBIAN_DIR = /usr/share/qt4/qt-symbian
ARM_DIR = /usr/arm-none-symbianelf

CC = arm-none-symbianelf-g++
AR = arm-none-symbianelf-ar cqs
LD = arm-none-symbianelf-ld


DEFINES = -DUNICODE -DQT_KEYPAD_NAVIGATION -DQT_SOFTKEYS_ENABLED -DQT_USE_MATH_H_FLOATS -D__PRODUCT_INCLUDE__=\<${EPOCROOT}/epoc32/include/variant/symbian_os.hrh\> -D__SYMBIAN32__ -D__MARM_INTERWORK__ -D_UNICODE -D__S60_50__ -D__S60_3X__ -D__SERIES60_3X__ -D__EPOC32__ -D__MARM__ -D__EABI__ -D__MARM_ARMV5__ -D__SUPPORT_CPP_EXCEPTIONS__ -D__GCCE__ -DUNICODE -D_STLP_NO_EXCEPTION_HEADER -DQT_GUI_LIB -DQT_CORE_LIB

CXXFLAGS = -pipe --include=${EPOCROOT}/epoc32/include/gcce/gcce.h -march=armv5t -mapcs -mthumb-interwork -nostdinc -c -msoft-float -T script -x c++ -fexceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG -fno-unit-at-a-time -fvisibility-inlines-hidden 
CXXFLAGS += -DLIBICONV_PLUG -DDO_ICONV_CAST  -DROOTPATH=\"\" -fvisibility-inlines-hidden

INCPATH = -I${QTSYMBIAN_DIR}/mkspecs/symbian/linux-gcce -I. -I${QTSYMBIAN_DIR}/include -I${EPOCROOT}/epoc32/include/ -I${EPOCROOT}/epoc32/include/variant -I${EPOCROOT}/epoc32/include/stdapis -I${EPOCROOT}/epoc32/include/gcce -I${EPOCROOT}/epoc32/include/stdapis/sys -I${EPOCROOT}/epoc32/include/stdapis/stlport -I${EPOCROOT}/epoc32-s60/include -I${EPOCROOT}/epoc32/include/oem -I${EPOCROOT}/epoc32/include/middleware -I${EPOCROOT}/epoc32/include/domain/middleware -I${EPOCROOT}/epoc32/include/osextensions -I${EPOCROOT}/epoc32/include/domain/osextensions -I${EPOCROOT}/epoc32/include/domain/osextensions/loc -I${EPOCROOT}/epoc32/include/domain/middleware/loc -I${EPOCROOT}/epoc32/include/domain/osextensions/loc/sc -I${EPOCROOT}/epoc32/include/domain/middleware/loc/sc -I. 

DISABLE_WARNINGS = -w

CFLAGS = $(DEFINES) $(CXXFLAGS) $(INCPATH) $(DISABLE_WARNINGS)

LDFLAGS = --target1-abs --no-undefined --nostdlib \
-L${ARM_DIR}/lib/gcc/arm-none-symbianelf/4.4.1/ -L${ARM_DIR}/lib/gcc/ -L${ARM_DIR}/arm-none-symbianelf/lib/arm-none-symbianelf/4.4.1/ -L${ARM_DIR}/arm-none-symbianelf/lib/  \
-L$(ROOTDIR)/libs/symbian \
-L${EPOCROOT}/epoc32/release/armv5/udeb/ -L${EPOCROOT}//epoc32/release/armv5/lib -L${EPOCROOT}/epoc32/release/armv5/urel -L${QTSYMBIAN_DIR}/lib -L${EPOCROOT}/epoc32/release/armv5/udeb \
-Tdata 0x700000 -Ttext 0x80000 --shared --soname fbreader\{00040701\}\[e87cc83c\].exe --entry=_E32Startup -u _E32Startup


MOC = moc-symbian
QTINCLUDE = -I${QTSYMBIAN_DIR}/include/QtCore -I${QTSYMBIAN_DIR}/include/QtGui
EXTERNAL_INCLUDE = -I$(ROOTDIR)/libs/symbian/include
ZLSHARED = no
TARGET_STATUS = symbian

UILIBS = -lQtGui

RM = rm -rvf
RM_QUIET = rm -rf
