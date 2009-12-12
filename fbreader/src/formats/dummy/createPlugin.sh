#!/bin/sh

if [ $# != 3 ]; then
	echo "usage: $0 <short_format_name> <camel_cased_format_name> <upper_cased_format_name>";
	exit 0;
fi;

if mkdir ../$1; then
	for file in Dummy*.h Dummy*.cpp; do
		sed "s/Dummy/$2/g" $file | sed "s/DUMMY/$3/g" > ../$1/`echo $file | sed "s/Dummy/$2/"`;
	done
fi;
