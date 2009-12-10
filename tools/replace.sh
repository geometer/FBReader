#!/bin/sh

if [ $# -ne 2 ]; then
	echo -e "usage:\n\t$0 <from_pattern> <to_pattern>";
	exit 0;
fi

echo will replace $1 by $2

for file in `find . -name "*.h" -o -name "*.cpp"`; do
	if grep -q "$1" $file; then
		sed -i "s/$1/$2/" $file;
	fi;
done
