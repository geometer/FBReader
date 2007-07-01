TMP_DIR=$(CURDIR)/tmp
DATA_DIR = $(TMP_DIR)/data

UI_TYPE=qtopia-$(RESOLUTION)

all: libzlibrary-package fbreader-package

libzlibrary-package: build
	@mkdir $(TMP_DIR)
	@mkdir $(DATA_DIR)
	@make -C zlibrary/core TARGET_ARCH=zaurus UI_TYPE=$(UI_TYPE) DESTDIR=$(DATA_DIR) do_install
	@make -C zlibrary/text TARGET_ARCH=zaurus UI_TYPE=$(UI_TYPE) DESTDIR=$(DATA_DIR) do_install
	@make -C zlibrary/ui TARGET_ARCH=zaurus UI_TYPE=$(UI_TYPE) DESTDIR=$(DATA_DIR) do_install
	@sed \
		-e "s#@VERSION@#`cat zlibrary/VERSION`#" \
		-e "s#@RESOLUTION@#$(RESOLUTION)#" \
		-e "s#@SIZE@#`du -s -b $(DATA_DIR) | cut -f1`#" \
		distributions/zaurus-ipk/libzlibrary.control > $(TMP_DIR)/control
	@./tools/build_ipk.sh $(TMP_DIR)
	@rm -rf $(TMP_DIR)

fbreader-package: build
	@mkdir $(TMP_DIR)
	@mkdir $(DATA_DIR)
	@make -C fbreader TARGET_ARCH=zaurus UI_TYPE=$(UI_TYPE) DESTDIR=$(DATA_DIR) RESOLUTION=$(RESOLUTION) do_install
	@sed \
		-e "s#@VERSION@#`cat fbreader/VERSION`#" \
		-e "s#@RESOLUTION@#$(RESOLUTION)#" \
		-e "s#@SIZE@#`du -s -b $(DATA_DIR) | cut -f1`#" \
		distributions/zaurus-ipk/fbreader.control > $(TMP_DIR)/control
	@./tools/build_ipk.sh $(TMP_DIR)
	@rm -rf $(TMP_DIR)

build:
	@make TARGET_ARCH=zaurus UI_TYPE=$(UI_TYPE)
