#!/bin/sh

if [ $# -ne 2 ]; then
	echo -e "usage:\n\t$0 <from_pattern> <to_pattern>";
	exit 0;
fi

echo will replace $1 by $2

for file in `find . -type f -a -name "*.h" -o -name "*.cpp" -o -name "*.M"`; do
	if grep -q "$1" $file; then
		sed -e s/$1/$2/ -i \"\" $file;
	fi;
done
