#!/bin/sh

if [ $# != 2 ]; then
	echo "usage: ";
	exit 0;
fi;

if mkdir ../$1; then
	for file in Dummy*.h Dummy*.cpp; do
		sed "s/Dummy/$2/g" $file | sed "s/DUMMY/$2/g" > ../$1/`echo $file | sed "s/Dummy/$2/"`;
	done
fi;
