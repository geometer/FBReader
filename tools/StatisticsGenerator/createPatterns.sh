#! /bin/bash

if [ $# != 2 ]; then
	echo "Usage: $0 [Directory] [2 symbols language code]";
	exit 1;
fi

if ! [ -d $1 ]; then
    echo "Error: $1 is not directory or doesn't exist";
    exit 1;
fi

GENERATORPATH=$(pwd) >> /dev/null

pushd $1 >> /dev/null

if ! [ -e ./encodings ]; then
	echo "Configuration file \"encodings\" doesn't exist";
	popd >> /dev/null;
	exit 1;	
else
	echo "Patterns for directory $1 is creating"
	if ! [ -d ./temp ]; then
		mkdir temp
	fi
	SOURCE_ENCODING=$(cat ./encodings | line);
	for TARGET_ENCODING in $(cat ./encodings) 
	do  
		echo "$2_$TARGET_ENCODING"
		for txtfile in *.txt
		do
    		if [ $TARGET_ENCODING != $SOURCE_ENCODING ]; then
				iconv -c -f $SOURCE_ENCODING -t $TARGET_ENCODING $txtfile > tmp;
				mv tmp "./temp/$TARGET_ENCODING"_"$txtfile";
				#echo "./temp/$TARGET_ENCODING"_"$txtfile";
			else
				cp $txtfile ./temp/
			fi
		done
		$GENERATORPATH/PatternGenerator ./temp/
		mv ./temp/pattern.stat $GENERATORPATH/patterns/$2_$TARGET_ENCODING
		rm ./temp/* 
	done  	
	rmdir temp
fi

popd >> /dev/null;
exit 0 
