#!/bin/sh

do_build() {
	echo -n "Building $1 ($2) ...";
	TARGET_ARCH=$1 UI_TYPE=$2 make clean 1> /dev/null 2>&1;
	if ! TARGET_ARCH=$1 UI_TYPE=$2 make package 1> $1\($2\).log 2>&1; then
		echo " failure";
	else
		echo " OK"
		rm $1\($2\).log;
	fi
	TARGET_ARCH=$1 UI_TYPE=$2 make clean 1> /dev/null 2>&1;
}

if [ -r makefiles/target.mk ]; then
  mv makefiles/target.mk makefiles/__target.mk
fi

do_build opensimpad-0.9.0 opie
do_build qvfb opie
#do_build maemo maemo
do_build desktop gtk
do_build desktop qt
do_build zaurus qtopia
do_build pdaxrom-1.1.0rc10 gtk
do_build pdaxrom-1.1.0rc10 qt
do_build pdaxrom-1.1.0rc11 gtk
do_build pdaxrom-1.1.0rc11 qt
#do_build palm palm

if [ -r makefiles/__target.mk ]; then
  mv makefiles/__target.mk makefiles/target.mk
fi
