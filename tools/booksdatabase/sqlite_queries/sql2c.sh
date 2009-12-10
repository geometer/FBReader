#! /bin/bash


cat $1 | \
	sed ': start s/\"/:SEMICOLON:/; t start;' | \
	sed ': start s/:SEMICOLON:/\\\"/; t start;' | \
	sed 's/.*/\"& \" \\/;' | \
	sed -r ': start s/\[\[|\]\]/\"/; t start;'

