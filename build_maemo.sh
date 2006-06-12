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

if [ "$1" == "maemo" -o "$1" == "maemo2" ]; then
  do_build maemo $1
else
  echo usage:
	echo "  $0 [maemo | maemo2]"
fi
