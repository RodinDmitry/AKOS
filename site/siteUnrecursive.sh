#!/bin/bash

echo "/" >links

MAINDIR="$1"
DIR=""

mkdir -p "$MAINDIR"

while read HREF; do
    DIR=$MAINDIR$HREF
    echo $DIR
    mkdir -p $DIR
    echo "GET $HREF HTTP/1.0 \n\n" | ./netcat $1 80 > webpage
    cp webpage $DIR$webpage
    
    awk 'match($0, /href=\"[^"]+\">/){print substr($0, RSTART, RLENGTH)}' webpage >templinks
    awk 'match($0, /\"\/[^"]*\"/){print substr($0, RSTART+1, RLENGTH-2)}' templinks > newlinks
	rm templinks
    while read NHREF; do
        if ! grep -q "$NHREF" links ; then
            echo $NHREF >> links
        fi
    done < newlinks
done < links

rm links 
rm webpage
rm newlinks
rm newlinks

