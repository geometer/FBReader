#!/bin/sh

do_build() {
	echo -n "Building $1 ($2) ...";
	make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release clean 1> /dev/null 2>&1;
	if ! make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release packages 1> $1\($2\).log 2>&1; then
		echo " failure";
	else
		echo " OK"
		rm $1\($2\).log;
	fi
	make TARGET_ARCH=$1 UI_TYPE=$2 TARGET_STATUS=release clean 1> /dev/null 2>&1;
}

if [ "$1" != "maemo" -a "$1" != "maemo2" ]; then
  echo usage:
  echo "  $0 [maemo | maemo2]"
  exit 1;
fi

SDK=`ls /targets/links/scratchbox.config -l | sed -e "s/.\\+\\/\\(.\\+\\)\\.config$/\\1/"`

if [ $1 = "maemo" ]; then
  EXPECTED_SDK=SDK_ARM
else
  EXPECTED_SDK=SDK_ARMEL
fi

if [ "$SDK" != "$EXPECTED_SDK" ]; then
  echo Incorrect target: $SDK, $EXPECTED_SDK is expected
  echo run \'sbox-config -st $EXPECTED_SDK\'
  exit 1;
fi

do_build maemo $1
