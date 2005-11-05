#!/bin/sh

export DIR=fbreader-`cat fbreader/VERSION`
mkdir $DIR
cp -r Makefile build_all.sh zlibrary fbreader makefiles README $DIR

rm -rf `find $DIR -name ".svn"`
cd $DIR
make distclean
cd ..

tar czf fbreader-sources-`cat fbreader/VERSION`.tgz $DIR

rm -rf $DIR
