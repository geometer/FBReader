#!/bin/sh

if [ "$1" == "" ]; then
	part=fbreader
else
	part=$1
fi

for file in ../../$part/data/resources/*.xml; do
	xsltproc clean.xslt $file > `basename $file`;
done
for file in *.xml; do
	diff $file en.xml > `basename $file .xml`.diff;
done
rm *.xml
