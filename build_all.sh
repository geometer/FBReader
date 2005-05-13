#!/bin/sh

TARGETS="desktop-gtk desktop-qt zaurus-cacko zaurus-pdaxrom-qt zaurus-pdaxrom-gtk palm"

mv makefiles/target.mk makefiles/__target.mk

make clean 1> /dev/null 2>&1;
for target in $TARGETS; do
	echo -n "Building $target ...";
	if ! TARGET=$target make 1> $target.log 2>&1; then
		echo " failure";
		break;
	fi
	echo " OK"
	make clean 1> /dev/null 2>&1;
	rm $target.log;
done

mv makefiles/__target.mk makefiles/target.mk
