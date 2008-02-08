#!/bin/sh

if [ $# != 2 ]; then
	echo -e "usage\n  $0 <platform> <install_dir>";
	exit 0;
fi;

case "$1" in
	desktop|maemo|openzaurus_640x480|pdaxrom|pepperpad3|zaurus_640x480)
		platform=desktop;
		;;
	moto)
		platform=desktop;
		;;
	opensimpad-0.9.0|qvfb)
		platform=640x480;
		;;
	openzaurus_240x320|pma400|zaurus_240x320)
		platform=240x320;
		;;
	win32)
		platform=win32;
		;;
	*)
		echo "$0: unknown platform \"$1\"";
		exit 0;
		;;
esac;
installdir=$2;

for file in data/help/MiniHelp.$platform.*.fb2; do
	lang=`echo $file | cut -d . -f 3`;
	install -m 0644 $file $installdir/MiniHelp.$lang.fb2
done
