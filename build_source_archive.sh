#!/bin/sh

export DIR=fbreader-`cat fbreader/VERSION`
mkdir $DIR
cp -r Makefile zlibrary zldictionary fbreader makefiles README.build $DIR

rm -rf `find $DIR -name ".svn"`
rm -rf $DIR/fbreader/desktop/text
cd $DIR
make distclean
cd ..

tar czf fbreader-sources-`cat fbreader/VERSION`.tgz $DIR

rm -rf $DIR
