#!/bin/bash
generateQuery()
{
	echo -e "GET " $2 " HTTP/1.1\nHost: "\ $1 "\nAccept: text/html\nConnection: close\n\n" > Query
}

echo "/" > links

HOME=$1
NEWDIR=e

mkdir -p $HOME

while read line; do
	NEWDIR=$HOME$line
	mkdir -p $NEWDIR
	#generateQuery $1 $line
	#netcat $1 80 < Query > webpage
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
