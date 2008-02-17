#!/bin/sh

if [ $# != 2 ]; then
	echo -e "usage\n  $0 <platform> <install_dir>";
	exit 0;
fi;

case "$1" in
	desktop|win32|pepperpad3)
		toolbar_file=data/default/toolbar.full.xml
		;;
	zaurus_640x480)
		toolbar_file=data/default/toolbar.zaurus.xml
		;;
	*)
		toolbar_file=data/default/toolbar.short.xml
		;;
esac;

case "$1" in
	maemo|zaurus_640x480|zaurus_240x320|pma400)
		menubar_file=data/default/menubar.xml
		;;
	moto)
		menubar_file=data/default/menubar.moto.xml
		;;
	*)
		;;
esac;

if [ "$toolbar_file" != "" ]; then
	install -m 0644 $toolbar_file $2/toolbar.xml;
fi;

if [ "$menubar_file" != "" ]; then
	install -m 0644 $menubar_file $2/menubar.xml;
fi;
