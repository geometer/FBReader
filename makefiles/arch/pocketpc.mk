SHAREDIR = %APPLICATION_PATH%\\\\share
IMAGEDIR = $(SHAREDIR)\\\\icons
APPIMAGEDIR = $(IMAGEDIR)
FILENAMEDELIMITER = "\\\\"
PATHDELIMITER = ";"
EOLCHARS = "\\r\\n"

ZLSHARED = no

PREFIX = /usr/ppc/arm-wince-cegcc/bin/
CC = $(PREFIX)gcc
AR = $(PREFIX)ar rsu
LD = $(PREFIX)g++
RESOURCE_COMPILER = /usr/arm-wince-mingw32ce/bin/arm-wince-mingw32ce-windres

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DUNICODE -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0501 -DWINVER=0x0500 -DNOMINMAX
#LDFLAGS = -mwindows

#UILIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng12 -ljpeg -lungif -ltiff
UILIBS = -lcoredll -lpng -ljpeg -lungif -ltiff
XML_LIB = -lexpat
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
