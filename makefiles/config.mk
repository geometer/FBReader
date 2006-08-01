include $(ROOTDIR)/makefiles/platforms.mk

MAKE = ROOTDIR=$(ROOTDIR) make
LIBMAKE = BUILD_SHARED_LIBRARY=yes $(MAKE)

include $(ROOTDIR)/makefiles/arch/$(TARGET_ARCH).mk

ifeq "$(BUILD_SHARED_LIBRARY)" "yes"
	CFLAGS += -fPIC
endif

ifeq "$(TARGET_STATUS)" "release"
	CFLAGS += -O3
	LDFLAGS += -s
endif
ifeq "$(TARGET_STATUS)" "debug"
	CFLAGS += -O0 -g
endif
ifeq "$(TARGET_STATUS)" "profile"
	CFLAGS += -O3 -g -pg
endif

ZINCLUDE = -I $(ROOTDIR)/zlibrary/include -I $(ROOTDIR)/zldictionary/include
