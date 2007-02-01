INSTALLDIR = .
SHAREDIR = %APPLICATION_PATH%\\\\share
IMAGEDIR = $(INSTALLDIR)/share/%APPLICATION_NAME%/icons
PATHDELIMITER = \\\\

BUILD_SHARED_LIBRARY = no

CC = i586-mingw32msvc-gcc
AR = i586-mingw32msvc-ar rsu
LD = i586-mingw32msvc-g++
RESOURCE_COMPILER = i586-mingw32msvc-windres

CFLAGS = -pipe -fno-exceptions -Wall -Wno-ctor-dtor-privacy -W -I /home/geometer/win32/my/include -D_WIN32_IE=0x0500
LDFLAGS = -mwindows -L /home/geometer/win32/my/lib
# LDFLAGS = -Wl,-rpath,$(LIBDIR)

UILIBS = -lgdi32 -lcomctl32
EXPATLIBS = -lexpat -lz -liconv
ENCALIBS = -lenca
BZIP2LIBS = -lbz2

RM = rm -rvf
RM_QUIET = rm -rf

BUILD_RESOURCE_OBJECT = yes

.resources:
	@echo -n "Creating resource object..."
	@echo "ApplicationIcon ICON icons/win32/$(TARGET).ico" > $(TARGET).rc
	@if [ -d icons/win32/$(TARGET) ]; then \
		for file in icons/win32/$(TARGET)/*.bmp; do \
			echo `basename $$file .bmp` BITMAP $$file >> $(TARGET).rc; \
		done; \
	fi
	@$(RESOURCE_COMPILER) $(TARGET).rc -o src/$(TARGET)_rc.o
	@$(RM_QUIET) $(TARGET).rc
	@echo " OK"
