include $(ROOTDIR)/makefiles/arch/unix.mk

SDK_DIR = /usr/local/arm/3.4.1
SDK_BIN_DIR = $(SDK_DIR)/arm-linux/bin

CC = $(SDK_BIN_DIR)/gcc
AR = $(SDK_BIN_DIR)/ar rsu
LD = $(SDK_BIN_DIR)/g++

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DLIBICONV_PLUG -I $(ROOTDIR)/../ebr/ext/include
LDFLAGS = -L $(ROOTDIR)/../ebr/ext/lib

XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

RM = rm -rvf
RM_QUIET = rm -rf
