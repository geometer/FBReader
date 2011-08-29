#!/bin/sh

if [ $# != 3 ]; then
	echo -e "usage\n  $0 <platform> <ui_type> <install_dir>";
	exit 0;
fi;

case "$1" in
	desktop|win32)
		;;
	macosx)
		;;
	maemo)
		case "$2" in
			qml)
				config_file=data/default/config.maemo6.xml
				;;
			maemo5)
				config_file=data/default/config.maemo5.xml
				;;
			*)
				config_file=data/default/config.maemo.xml
				;;
			esac;
		;;
	pepperpad3)
		;;
	zaurus_640x480)
		;;
	*)
		;;
esac;

if [ "$config_file" != "" ]; then
	install -m 0644 $config_file $3/config.xml;
fi;
