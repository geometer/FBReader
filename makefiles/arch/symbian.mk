ifeq "$(INSTALLDIR)" ""
  INSTALLDIR=/usr
endif
SHAREDIR_MACRO=!!
IMAGEDIR = !!\\\\pixmaps
APPIMAGEDIR = !!\\\\pixmaps\\\\FBReader

SDK_ROOT = /opt/QtSDK/Symbian/SDKs/Symbian1
ARM_DIR = /usr/arm-none-symbianelf
#TODO: it should be generated automatically
#00040701 => 4.07(1)
#00009900 => 0.99(0)
SYMBIAN_VERSION=00009900

CC = arm-none-symbianelf-g++
AR = arm-none-symbianelf-ar cqs
LD = arm-none-symbianelf-ld


DEFINES = -DUNICODE -DQT_KEYPAD_NAVIGATION -DQT_SOFTKEYS_ENABLED -DQT_USE_MATH_H_FLOATS -D__PRODUCT_INCLUDE__=\<${SDK_ROOT}/epoc32/include/variant/symbian_os.hrh\> -D__SYMBIAN32__ -D__MARM_INTERWORK__ -D_UNICODE -D__S60_50__ -D__S60_3X__ -D__SERIES60_3X__ -D__EPOC32__ -D__MARM__ -D__EABI__ -D__MARM_ARMV5__ -D__SUPPORT_CPP_EXCEPTIONS__ -D__GCCE__ -DUNICODE -D_STLP_NO_EXCEPTION_HEADER -DQT_GUI_LIB -DQT_CORE_LIB

CXXFLAGS = -pipe --include=${SDK_ROOT}/epoc32/include/gcce/gcce.h -march=armv5t -mapcs -mthumb-interwork -nostdinc -c -msoft-float -T script -x c++ -fexceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG -fno-unit-at-a-time -fvisibility-inlines-hidden 
CXXFLAGS += -DLIBICONV_PLUG -DDO_ICONV_CAST  -DROOTPATH=\"\" -fvisibility-inlines-hidden

CXXFLAGS += -D__SYMBIAN__

#-I${SDK_ROOT}/epoc32/include/platform is needed for S^3 (and this dir absent in S^1) SDK, for using remconcoreapi (for capturing volume keys)

INCPATH = -I. \
-I$(ROOTDIR)/libs/symbian/include \
-I${SDK_ROOT}/mkspecs/symbian/linux-gcce  \
-I${SDK_ROOT}/epoc32/include/ \
-I${SDK_ROOT}/epoc32/include/platform \
-I${SDK_ROOT}/epoc32/include/variant \
-I${SDK_ROOT}/epoc32/include/stdapis \
-I${SDK_ROOT}/epoc32/include/stdapis/sys \
-I${SDK_ROOT}/epoc32/include/stdapis/stlport \
-I${SDK_ROOT}/epoc32/include/oem \
-I${SDK_ROOT}/epoc32/include/middleware \
-I${SDK_ROOT}/epoc32/include/osextensions \
-I${SDK_ROOT}/epoc32/include/domain/middleware \
-I${SDK_ROOT}/epoc32/include/domain/osextensions \
-I${SDK_ROOT}/epoc32/include/domain/osextensions/loc \
-I${SDK_ROOT}/epoc32/include/domain/osextensions/loc/sc \
-I${SDK_ROOT}/epoc32/include/domain/middleware/loc \
-I${SDK_ROOT}/epoc32/include/domain/middleware/loc/sc

DISABLE_WARNINGS = -w

CFLAGS = $(DEFINES) $(CXXFLAGS) $(INCPATH) $(DISABLE_WARNINGS)

LDFLAGS = --target1-abs --no-undefined --nostdlib \
-Tdata 0x700000 -Ttext 0x80000 --shared --soname fbreader\{$(SYMBIAN_VERSION)\}\[e87cc83c\].exe --entry=_E32Startup -u _E32Startup
#TODO: does it need in LDFLAGS --  ??

LDFLAGS += \
-L$(ROOTDIR)/libs/symbian \
-L${ARM_DIR}/lib/gcc/arm-none-symbianelf/4.4.1/ \
-L${ARM_DIR}/lib/gcc/ \
-L${ARM_DIR}/arm-none-symbianelf/lib/arm-none-symbianelf/4.4.1/ \
-L${ARM_DIR}/arm-none-symbianelf/lib/  \
-L${SDK_ROOT}/epoc32/release/armv5/udeb/ \
-L${SDK_ROOT}/epoc32/release/armv5/lib \
-L${SDK_ROOT}/epoc32/release/armv5/urel \
-L${SDK_ROOT}/epoc32/release/armv5/udeb 



MOC = moc-symbian
QTINCLUDE = -I${SDK_ROOT}/include
EXTERNAL_INCLUDE = -I$(ROOTDIR)/libs/symbian/include
ZLSHARED = no
TARGET_STATUS = symbian

UILIBS = -lQtGui

RM = rm -rvf
RM_QUIET = rm -rf
