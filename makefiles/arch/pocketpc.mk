SHAREDIR = %APPLICATION_PATH%\\\\share
IMAGEDIR = $(SHAREDIR)\\\\icons
APPIMAGEDIR = $(IMAGEDIR)
FILENAMEDELIMITER = "\\\\"
PATHDELIMITER = ";"

BUILD_SHARED_LIBRARY = no

PREFIX = arm-wince-mingw32ce-
CC = $(PREFIX)gcc
AR = $(PREFIX)ar rsu
LD = $(PREFIX)g++
RESOURCE_COMPILER = $(PREFIX)windres

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DDONT_USE_ICONV
#-DUNICODE -D_WIN32_IE=0x0501 -D_WIN32_WINNT=0x0501 -DWINVER=0x0500
#LDFLAGS = -mwindows

UILIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng12 -ljpeg -lungif -ltiff
CONVERTER_LIB = -liconv
XML_LIB = -lexpat
ENCA_LIB = -lenca
ARCHIVER_LIB = -lz -lbz2

RM = rm -rvf
RM_QUIET = rm -rf

BUILD_RESOURCE_OBJECT = yes

.resources:
	@echo -n "Creating resource object..."
	@echo "ApplicationIcon ICON icons/win32/$(TARGET).ico" > $(TARGET).rc
	@$(RESOURCE_COMPILER) $(TARGET).rc -o src/$(TARGET)_rc.o
	@$(RM_QUIET) $(TARGET).rc
	@echo " OK"
