#!/bin/sh

version=`cat fbreader/VERSION`
tmpdir=fbreader-$version

if [ "$1" == "-non-GPL" ]; then
	distdir=distributions-nonGPL
	pkgdir=packages-nonGPL
	prepare_nonGPL=true
	shift;
else
	distdir=distributions
	pkgdir=packages
	prepare_nonGPL=false
fi

if [ $# != 1 ]; then
	echo "usage:"
	echo "  $0 [-non-GPL] <architecture>"
	echo "or"
	echo "  $0 [-non-GPL] all"
	echo ""
	echo "available architectures are:"
	for pkgtype in $distdir/*; do
		for archtype in $pkgtype/*; do
			echo "  `basename $archtype`-`basename $pkgtype`";
		done;
	done;
	exit 1;
fi

create_tmpdir() {
	mkdir $tmpdir
	cp -r Makefile build_packages.sh zlibrary fbreader makefiles README.build $distdir $tmpdir
	rm -rf `find $tmpdir -name ".svn"`
	make -C $tmpdir distclean 1> /dev/null 2>&1

	if [ "$prepare_nonGPL" == "true" ]; then
		pushd $tmpdir > /dev/null;
		echo -en "Removing Finnish localization... ";
		rm -rf fbreader/data/resources/fi.xml zlibrary/core/data/resources/fi.xml fbreader/data/help/MiniHelp.*.fi.fb2;
		echo OK;
		echo -en "Removing Swedish localization... ";
		rm -rf fbreader/data/resources/sv.xml zlibrary/core/data/resources/sv.xml fbreader/data/help/MiniHelp.*.sv.fb2;
		echo OK;
		echo -en "Removing Czech hyphenation patterns... ";
		zip -dq zlibrary/text/data/hyphenationPatterns.zip cs.pattern;
		echo OK;
		echo -en "Removing Indonesian hyphenation patterns... ";
		zip -dq zlibrary/text/data/hyphenationPatterns.zip id.pattern;
		echo OK;
		echo -en "Removing Qt-based interface... ";
		rm -rf zlibrary/ui/src/qt zlibrary/ui/src/qt4
		echo OK;
		popd > /dev/null;
	fi;
}

remove_tmpdir() {
	rm -rf $tmpdir
}

build_package() {
	make_package="make -f makefiles/packaging.mk -C $tmpdir DIST_DIR=$distdir"

	case "$2" in
		debian)
			case "$1" in
				maemo)
					/scratchbox/login sb-conf se SDK_ARM
					/scratchbox/login -d `pwd` $make_package ARCHITECTURE=$1 $2
					;;
				maemo2)
					/scratchbox/login sb-conf se SDK_ARMEL
					/scratchbox/login -d `pwd` $make_package ARCHITECTURE=$1 $2
					;;
				maemo4)
					/scratchbox/login sb-conf se CHINOOK_ARMEL
					/scratchbox/login -d `pwd` $make_package ARCHITECTURE=$1 $2
					;;
				*)
					$make_package ARCHITECTURE=$1 $2
					;;
			esac;
			mkdirhier $pkgdir/$1
			mv -f $tmpdir/*.deb $tmpdir/*.dsc $tmpdir/*.changes $tmpdir/*.tar.gz $pkgdir/$1
			;;
		ipk|debipk)
			$make_package ARCHITECTURE=$1 $2
			mkdirhier $pkgdir/$1
			mv -f $tmpdir/*.ipk $pkgdir/$1
			;;
		motopkg)
			$make_package ARCHITECTURE=$1 $2
			mkdirhier $pkgdir/$1
			mv -f $tmpdir/*.pkg $pkgdir/$1
			;;
		tarball)
			$make_package ARCHITECTURE=$1 $2
			mkdirhier $pkgdir/$1
			mv -f $tmpdir/*.tgz $pkgdir/$1
			;;
		nsi)
			$make_package ARCHITECTURE=$1 $2
			mkdirhier $pkgdir/$1
			mv -f $tmpdir/*.exe $pkgdir/$1
			;;
		*)
			echo no rule is defined for package type ''$2'';
			;;
	esac;
}

if [ $1 == all ]; then
	create_tmpdir
	for pkgtype in $distdir/*; do
		for archtype in $pkgtype/*; do
			build_package `basename $archtype` `basename $pkgtype`;
		done;
	done;
	remove_tmpdir
else
	archtype=`echo $1 | cut -d "-" -f 1`;
	pkgtype=`echo $1 | cut -d "-" -f 2`;
	extra=`echo $1 | cut -d "-" -f 3`;

	if [ "$pkgtype" != "" -a "$extra" == "" -a -d $distdir/$pkgtype/$archtype ]; then
		create_tmpdir
		build_package $archtype $pkgtype
		remove_tmpdir
	else 
		echo "unknown architecture: $1"
	fi;
fi;
