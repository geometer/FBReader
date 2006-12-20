#!/bin/sh

build_archive() {
	version=`cat $1/VERSION`
	dir=$1-$version
	mkdir $dir
	cp -r $2 $dir
	rm -rf `find $dir -name ".svn"`
	pushd $dir; make distclean; popd
	tar czf $1-sources-$version.tgz $dir
	rm -rf $dir
}

build_archive zlibrary "Makefile zlibrary makefiles"
build_archive fbreader "Makefile zlibrary fbreader makefiles README.build"
build_archive GeometricCalculator "Makefile zlibrary GeometricCalculator makefiles"
