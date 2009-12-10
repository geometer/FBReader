#!	/bin/bash

rm ./patterns/*_*
for dir in ./patterns/base/*
do
	LANGUAGE_NAME=${dir:16:2}
	case $LANGUAGE_NAME in
        "ch")
        LANGUAGE_NAME="zh"
        ;;
        "po")
        LANGUAGE_NAME="pt"
        ;;
        "sp")
        LANGUAGE_NAME="es"
        ;;
		"ge")
        LANGUAGE_NAME="de"
        ;;
		"cz")
        LANGUAGE_NAME="cs"
        ;;
		"es")
        LANGUAGE_NAME="eo"
        ;;
		"gr")
        LANGUAGE_NAME="el"
        ;;
		"sw")
        LANGUAGE_NAME="sv"
        ;;
		"tu")
        LANGUAGE_NAME="tr"
        ;;
    esac
	./createPatterns.sh $dir $LANGUAGE_NAME
	#echo $LANGUAGE_NAME
done

pushd ./patterns >> /dev/null;
zip -r ../../../zlibrary/core/data/languagePatterns.zip *_*
#mv *_* ../localPatterns
popd ./patterns >> /dev/null;
