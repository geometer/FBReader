#! /bin/bash

topAmount()
{
	echo $3
	./StatisticsMatcher ./localPatterns/ $2 $1 >> $3
	./StatisticsMatcher ./localPatterns/ $2 $1 - 100 >> $3
	./StatisticsMatcher ./localPatterns/ $2 $1 - 1000 >> $3
}

topVolume()
{
	echo $3
	./StatisticsMatcher ./localPatterns/ $2 $1 % 50 >> $3
	./StatisticsMatcher ./localPatterns/ $2 $1 % 20 >> $3
}

rm ./data/*
for file in ./testExamples/*
do
	FILENAME=${file##*\/}
	BASE=${FILENAME%.*}
	TO1=./data/$BASE.chi
	TO2=./data/$BASE.cor
	TO3=./data/$BASE.mat
	echo $TO
	topAmount chi $file $TO1
	topVolume chi $file $TO1
	topAmount cor $file $TO2
	topVolume cor $file $TO2
	topAmount mat $file $TO3
done
