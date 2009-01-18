include $(ROOTDIR)/makefiles/arch/unix.mk

INSTALLDIR = /mnt/ext2
ZLSHARED = no

SDK_DIR = /usr/local/arm/3.4.1
SDK_BIN_DIR = $(SDK_DIR)/arm-linux/bin

CC = $(SDK_BIN_DIR)/gcc
AR = $(SDK_BIN_DIR)/ar rsu
LD = $(SDK_BIN_DIR)/g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG -I $(ROOTDIR)/../ebr/microwin/src/include -I $(ROOTDIR)/../ebr/ext/include -DXMLCONFIGHOMEDIR=\"/mnt/ext2\" -DDO_ICONV_CAST
LDFLAGS = -L $(ROOTDIR)/../ebr/microwin/lib/jpeg -L $(ROOTDIR)/../ebr/microwin/lib/mfont -L $(ROOTDIR)/../ebr/microwin/src/lib -L $(ROOTDIR)/../ebr/ext/lib

UILIBS = -lnano-X -ljpeg -lmfont

RM = rm -rvf
RM_QUIET = rm -rf
