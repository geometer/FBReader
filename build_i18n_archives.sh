#!/bin/sh

VERSION=`cat fbreader/VERSION`

build_language_archive() {
	lang=$1
	dir=FBReader-i18n-$lang-$VERSION
	mkdir $dir
	cp fbreader/data/resources/$lang.xml $dir/FBReader-$lang.xml
	cp zlibrary/core/data/resources/$lang.xml $dir/zlibrary-$lang.xml
	cp fbreader/data/help/MiniHelp.desktop.$lang.fb2 $dir/MiniHelp.unix.$lang.fb2
	cp fbreader/data/help/MiniHelp.win32.$lang.fb2 $dir/MiniHelp.windows.$lang.fb2
	zip -r $dir.zip $dir
	rm -rf $dir
}

for file in fbreader/data/resources/*; do
	build_language_archive `basename $file .xml`;
done;
