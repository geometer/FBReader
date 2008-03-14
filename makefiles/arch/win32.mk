SHAREDIR = %APPLICATION_PATH%\\\\share
IMAGEDIR = $(SHAREDIR)\\\\icons
APPIMAGEDIR = $(IMAGEDIR)

ZLSHARED = no

CC = i586-mingw32msvc-gcc
AR = i586-mingw32msvc-ar rsu
LD = i586-mingw32msvc-g++
RESOURCE_COMPILER = i586-mingw32msvc-windres

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -DUNICODE -D_WIN32_IE=0x0501 -D_WIN32_WINNT=0x0501 -DWINVER=0x0500
LDFLAGS = -mwindows

UILIBS = -lgdi32 -lcomctl32 -lcomdlg32 -lpng12 -ljpeg -lungif -ltiff
XML_LIB = -lexpat
ARCHIVER_LIB = -lz -lbz2

RM = rm -rvf
RM_QUIET = rm -rf

BUILD_RESOURCE_OBJECT = yes

.resources:
	@echo -n "Creating resource object..."
	@echo "ApplicationIcon ICON data/icons/application/win32.ico" > $(TARGET).rc
	@echo "1 24 win32/manifest" >> $(TARGET).rc
	@$(RESOURCE_COMPILER) $(TARGET).rc -o src/$(TARGET)_rc.o
	@$(RM_QUIET) $(TARGET).rc
	@echo " OK"
