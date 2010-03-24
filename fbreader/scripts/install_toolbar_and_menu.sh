#!/bin/sh

if [ $# != 3 ]; then
	echo -e "usage\n  $0 <platform> <ui_type> <install_dir>";
	exit 0;
fi;

case "$1" in
	desktop|win32)
		toolbar_file=data/default/toolbar.desktop.xml
		fullscreen_toolbar_file=data/default/fullscreen_toolbar.desktop.xml
		;;
	macosx)
		toolbar_file=data/default/toolbar.macosx.xml
		;;
	maemo)
		case "$2" in
			maemo5)
		    toolbar_file=data/default/toolbar.maemo5.xml
				;;
			*)
		    toolbar_file=data/default/toolbar.maemo.xml
				;;
			esac;
		;;
	pepperpad3)
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
	zaurus_640x480|zaurus_240x320|pma400)
		menubar_file=data/default/menubar.xml
		;;
	maemo)
		case "$2" in
			maemo5)
				menubar_file=data/default/menubar.maemo5.xml
				;;
			*)
				menubar_file=data/default/menubar.xml
				;;
			esac;
		;;
	moto)
		menubar_file=data/default/menubar.moto.xml
		;;
	*)
		;;
esac;

if [ "$toolbar_file" != "" ]; then
	install -m 0644 $toolbar_file $3/toolbar.xml;
fi;

if [ "$fullscreen_toolbar_file" != "" ]; then
	install -m 0644 $fullscreen_toolbar_file $3/fullscreen_toolbar.xml;
fi;

if [ "$menubar_file" != "" ]; then
	install -m 0644 $menubar_file $3/menubar.xml;
fi;
