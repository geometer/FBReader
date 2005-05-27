#!/bin/sh

TARGETS="desktop-gtk desktop-qt zaurus-qtopia zaurus-pdaxrom-qt zaurus-pdaxrom-gtk palm"

if [ -r makefiles/target.mk ]
then
  mv makefiles/target.mk makefiles/__target.mk
  renamed=1
else
  renamed=""
fi

make clean 1> /dev/null 2>&1;
for target in $TARGETS; do
	echo -n "Building $target ...";
	if ! TARGET_ARCH=$target make 1> $target.log 2>&1; then
		echo " failure";
		break;
	fi
	echo " OK"
	make clean 1> /dev/null 2>&1;
	rm $target.log;
done

if [ -n "$renamed" ]
then
  mv makefiles/__target.mk makefiles/target.mk
fi
