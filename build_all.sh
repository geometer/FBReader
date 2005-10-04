#!/bin/sh

TARGETS="desktop-gtk desktop-qt zaurus-qtopia zaurus-pdaxrom-qt zaurus-pdaxrom-gtk qvfb-opie opensimpad-opie palm"

make clean 1> /dev/null 2>&1;

if [ -r makefiles/target.mk ]; then
  mv makefiles/target.mk makefiles/__target.mk
fi

for target in $TARGETS; do
	echo -n "Building $target ...";
	if ! TARGET_ARCH=$target make 1> $target.log 2>&1; then
		echo " failure";
		break;
	fi
	echo " OK"
	TARGET_ARCH=$target make clean 1> /dev/null 2>&1;
	rm $target.log;
done

if [ -r makefiles/__target.mk ]; then
  mv makefiles/__target.mk makefiles/target.mk
fi
