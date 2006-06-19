#!/bin/sh

do_build() {
	echo -n "Building $1 ($2) ...";
	make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release clean 1> /dev/null 2>&1;
	if ! make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release package 1> $1\($2\).log 2>&1; then
		echo " failure";
	else
		echo " OK"
		rm $1\($2\).log;
	fi
	make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release clean 1> /dev/null 2>&1;
}

do_build openzaurus opie
do_build openzaurus gpe
do_build opensimpad-0.9.0 opie
do_build qvfb opie
do_build desktop gtk
do_build desktop qt
do_build zaurus qtopia
do_build pdaxrom-1.1.0rc10 gtk
do_build pdaxrom-1.1.0rc10 qt
do_build pdaxrom-1.1.0rc11 gtk
do_build pdaxrom-1.1.0rc11 qt
do_build pma400 qtopia
