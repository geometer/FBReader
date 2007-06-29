TMP_DIR=$(CURDIR)/fbreader-desktop-tarball

all: tarball

clean:
	make clean

build: clean
	make -C zlibrary/core TARGET_ARCH=desktop UI_TYPE=dummy
	make -C zlibrary/text TARGET_ARCH=desktop UI_TYPE=dummy
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=gtk
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=qt
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=qt4
	make -C fbreader TARGET_ARCH=desktop UI_TYPE=dummy

install: build
	mkdir $(TMP_DIR)
	make -C zlibrary/core TARGET_ARCH=desktop UI_TYPE=dummy DESTDIR=$(TMP_DIR) do_install
	make -C zlibrary/text TARGET_ARCH=desktop UI_TYPE=dummy DESTDIR=$(TMP_DIR) do_install
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=gtk DESTDIR=$(TMP_DIR) do_install
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=qt DESTDIR=$(TMP_DIR) do_install
	make -C zlibrary/ui TARGET_ARCH=desktop UI_TYPE=qt4 DESTDIR=$(TMP_DIR) do_install
	make -C fbreader TARGET_ARCH=desktop UI_TYPE=dummy DESTDIR=$(TMP_DIR) do_install

tarball: install
	tar czf fbreader-`cat fbreader/VERSION`.tgz -C $(TMP_DIR) .
	rm -rf $(TMP_DIR)
