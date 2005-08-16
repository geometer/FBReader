#!/bin/sh

TARGETS="desktop-gtk desktop-qt zaurus-qtopia zaurus-pdaxrom-qt zaurus-pdaxrom-gtk palm"

if [ -r makefiles/target.mk ]; then
  mv makefiles/target.mk makefiles/__target.mk
fi

make clean 1> /dev/null 2>&1;
for target in $TARGETS; do
	echo -n "Building $target ...";
	if ! TARGET_ARCH=$target make 1> $target.log 2>&1; then
		echo " failed"
  else
    echo " OK"
    rm $target.log
	fi
	make clean 1> /dev/null 2>&1
done

if [ -r makefiles/__target.mk ]; then
  mv makefiles/__target.mk makefiles/target.mk
fi
