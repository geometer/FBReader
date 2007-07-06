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
	exit 1;
fi

build_package() {
	make_package="make -f makefiles/packaging.mk"

	case "$2" in
		debian)
			case "$1" in
				maemo)
					/scratchbox/login sbox-config -st SDK_ARM
					/scratchbox/login -d src/zaurus $make_package ARCHITECTURE=$1 .$2
					;;
				maemo2)
					/scratchbox/login sbox-config -st SDK_ARMEL
					/scratchbox/login -d src/zaurus $make_package ARCHITECTURE=$1 .$2
					;;
				*)
					$make_package ARCHITECTURE=$1 .$2
					;;
			esac;
			mkdirhier packages/$1
			mv -f *.deb *.dsc *.changes *.tar.gz packages/$1
			;;
		ipk|debipk)
			$make_package ARCHITECTURE=$1 .$2
			mkdirhier packages/$1
			mv -f *.ipk packages/$1
			;;
		tarball)
			$make_package ARCHITECTURE=$1 .$2
			mkdirhier packages/$1
			mv -f *.tgz packages/$1
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

#	do_build win32 win32
#	do_build openzaurus opie
#	do_build openzaurus gpe
#	do_build opensimpad-0.9.0 opie
#	do_build qvfb opie
