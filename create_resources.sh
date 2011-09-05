#!/bin/bash

DEST_DIR=resources

rm -rf $DEST_DIR

mkdir -p $DEST_DIR/zlibrary/
mkdir -p $DEST_DIR/zlibrary/default
cp -r zlibrary/core/data/encodings $DEST_DIR/zlibrary
cp -r zlibrary/core/data/resources $DEST_DIR/zlibrary
cp zlibrary/core/data/languagePatterns.zip $DEST_DIR/zlibrary
cp zlibrary/core/data/unicode.xml.gz $DEST_DIR/zlibrary
cp zlibrary/core/data/default/config.symbian.xml $DEST_DIR/zlibrary/default


cp -r zlibrary/text/data/hyphenationPatterns.zip $DEST_DIR/zlibrary
cp -r zlibrary/ui/data/keynames.symbian.xml $DEST_DIR/zlibrary

mv $DEST_DIR/zlibrary/default/config.symbian.xml $DEST_DIR/zlibrary/default/config.xml
mv $DEST_DIR/zlibrary/keynames.symbian.xml $DEST_DIR/zlibrary/keynames.xml


mkdir -p $DEST_DIR/FBReader
cp -r \
    fbreader/data/formats \
	fbreader/data/network \
	fbreader/data/$DEST_DIR \
	$DEST_DIR/FBReader/

mkdir -p $DEST_DIR/FBReader/help
HELP_FILES=fbreader/data/help/*symbian*fb2; 
for file in $HELP_FILES; do \
	new_file=$(echo $file|sed 's/.symbian//g'); 
	new_file=$(echo $new_file|sed 's/fbreader\/data\/help\///g');
	cp $file $DEST_DIR/FBReader/help/$new_file;
done


mkdir -p $DEST_DIR/FBReader/default
DEFAULTS="config external fullscreen_toolbar keymap menubar styles toolbar"
for filename in $DEFAULTS; do \
	cp fbreader/data/default/$filename.symbian.xml $DEST_DIR/FBReader/default/$filename.xml
done


mkdir -p $DEST_DIR/pixmaps
cp fbreader/data/icons/application/symbian.png $DEST_DIR/pixmaps/FBReader.png
cp fbreader/data/icons/application/symbian.svg $DEST_DIR/pixmaps/FBReader.svg

mkdir -p $DEST_DIR/pixmaps/FBReader
ICONS="booktree filetree toolbar"
for dirname in $ICONS; do \
	cp fbreader/data/icons/$dirname/symbian/* $DEST_DIR/pixmaps/FBReader/
done

echo "$DEST_DIR have been succesfully created at '$DEST_DIR/' folder"

#version
VERSION=`cat fbreader/VERSION`
sed "s/VERSION/$VERSION/" fbreader/data/formats/fb2/FBReaderVersion.ent > $DEST_DIR/FBReader/formats/fb2/FBReaderVersion.ent

