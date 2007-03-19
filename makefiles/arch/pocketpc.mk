SHAREDIR = %APPLICATION_PATH%\\\\share
IMAGEDIR = $(SHAREDIR)\\\\icons
APPIMAGEDIR = $(IMAGEDIR)
FILENAMEDELIMITER = "\\\\"
PATHDELIMITER = ";"

BUILD_SHARED_LIBRARY = no

CC = arm-wince-pe-gcc
AR = arm-wince-pe-ar rsu
LD = arm-wince-pe-g++
RESOURCE_COMPILER = arm-wince-pe-windres

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DUNICODE -D_WIN32_IE=0x0501 -D_WIN32_WINNT=0x0501 -DWINVER=0x0500
LDFLAGS = -mwindows

UILIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng12 -ljpeg -lungif -ltiff
EXPATLIBS = -lexpat -lz -liconv
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

RM = rm -rvf
RM_QUIET = rm -rf

BUILD_RESOURCE_OBJECT = yes

.resources:
	@echo -n "Creating resource object..."
	@echo "ApplicationIcon ICON icons/win32/$(TARGET).ico" > $(TARGET).rc
	@echo "1 24 win32/manifest" >> $(TARGET).rc
	@$(RESOURCE_COMPILER) $(TARGET).rc -o src/$(TARGET)_rc.o
	@$(RM_QUIET) $(TARGET).rc
	@echo " OK"
