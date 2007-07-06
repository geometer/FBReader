#!/bin/sh

if [ $# != 1 ]; then
	echo "usage:"
	echo "  $0 <architecture>"
	echo "or"
	echo "  $0 all"
	echo ""
	echo "available architectures are:"
	for pkgdir in distributions/*; do
		for archdir in $pkgdir/*; do
			echo "  `basename $archdir`-`basename $pkgdir`";
		done;
	done;
	echo "  source-archive";
	exit 1;
fi

build_package() {
	make_package="make -f makefiles/packaging.mk"

	case "$2" in
		debian)
			case "$1" in
				maemo)
					/scratchbox/login sbox-config -st SDK_ARM
					/scratchbox/login -d src/zaurus $make_package ARCHITECTURE=maemo .debian
					;;
				maemo2)
					/scratchbox/login sbox-config -st SDK_ARMEL
					/scratchbox/login -d src/zaurus $make_package ARCHITECTURE=maemo2 .debian
					;;
				*)
					$make_package ARCHITECTURE=desktop .debian
					;;
			esac;
			mkdirhier packages/$1
			mv -f *.deb *.dsc *changes *.tar.gz packages/$1
			;;
		tarball)
			echo no rule is defined for package type ''$2'';
			;;
		ipk)
			$make_package ARCHITECTURE=$1 .ipk
			mkdirhier packages/$1
			mv -f *.ipk packages/$1
			;;
		*)
			echo no rule is defined for package type ''$2'';
			;;
	esac;
}

if [ $1 == all ]; then
	for pkgdir in distributions/*; do
		for archdir in $pkgdir/*; do
			build_package `basename $archdir` `basename $pkgdir`;
		done;
	done;
	exit 1;
fi

archtype=`echo $1 | cut -d "-" -f 1`
pkgtype=`echo $1 | cut -d "-" -f 2`
extra=`echo $1 | cut -d "-" -f 3`

if [ "$pkgtype" != "" -a "$extra" == "" -a -d distributions/$pkgtype/$archtype ]; then
	build_package $archtype $pkgtype
	exit 1;
fi;

echo "unknown architecture: $1"

#VERSION = $(shell cat fbreader/VERSION)
#SRCDIR = fbreader-$(VERSION)
#
#all: source-archive maemo-debian maemo2-debian desktop-tarball desktop-debian zaurus-640x480-ipk zaurus-240x320-ipk pdaxrom-ipk pma400-ipk
#
#source-archive:
#	@echo -en "Building source archive..."
#	@mkdir $(SRCDIR)
#	@cp -r Makefile zlibrary fbreader makefiles README.build $(SRCDIR)
#	@rm -rf `find $(SRCDIR) -name ".svn"`
#	@make -C $(SRCDIR) distclean 1> /dev/null 2>&1
#	@tar czf fbreader-sources-$(VERSION).tgz $(SRCDIR)
#	@rm -rf $(SRCDIR)
#	@echo " OK"
#
#desktop-tarball:
#	@echo -en "Building desktop tarball package..."
#	@make -f distributions/tarball/desktop/Makefile 1> $@.log 2>&1
#	@make clean 1> /dev/null 2>&1
#	@rm $@.log
#	@echo " OK"
#
##	do_build win32 win32
##	do_build openzaurus opie
##	do_build openzaurus gpe
##	do_build opensimpad-0.9.0 opie
##	do_build qvfb opie
