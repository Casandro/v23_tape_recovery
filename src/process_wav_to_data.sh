#!/bin/bash

bail () {
	echo $*
	exit;
}


for x in *.c
do
	gcc -Wall -lm $x -o `basename $x .c` || bail "Fehler bei $x"
done


IN="$1"


tmp=`mktemp /tmp/XXXXXXX.raw`
sox "$IN" -c 1 -r 6800 -e floating-point -b 64 -t raw - | ./demod_double | sox -c 1 -r 3400 -e floating-point -b 64 -t raw - -r 19200 -t raw  $tmp
cp $tmp /tmp/temp.raw
./uart $tmp  > $tmp.timedoctets
wc -l $tmp.timedoctets
./split_to_to_files /tmp/split/ < $tmp.timedoctets
rm $tmp
rm $tmp.timedoctets
