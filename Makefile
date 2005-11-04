ROOTDIR = $(PWD)

include makefiles/platforms.mk

LIBDIR = zlibrary
APPDIR = fbreader

.lib:
	cd $(LIBDIR); make

.app:
	cd $(APPDIR); make

all: .lib .app

install:
	cd $(APPDIR); make install

package: all
	cd $(APPDIR); make package

clean:
	cd $(LIBDIR); make clean
	cd $(APPDIR); make clean

distclean: clean
	rm -vf *.tgz *.ipk *.deb *.prc

archives: clean
