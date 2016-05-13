#!/bin/bash
echo "/" > links

HOME=$1
NEWDIR=e

mkdir -p $HOME

while read line; do
	NEWDIR=$HOME$line
	mkdir -p $NEWDIR
	echo "GET $2 HTTP/1.1 \n\n" | nc $1 80 > webpage
	cp webpage $NEWDIR$webpage
	
	awk -f pars1.awk webpage > templinks
    awk -f pars2.awk templinks > newlinks
	while read newLine; do
		if ! grep -q "$newLine" links ;  then
			echo $newLine >> links
		fi
	done < newlinks
done < links

rm links
rm templinks
rm newlinks
rm webpage
