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
OUT="`basename "$1" .wav`.zip"



tmp=`mktemp /tmp/XXXXXXX.raw`
sox "$IN" -c 1 -r 6800 -e floating-point -b 64 -t raw - sinc 300 -n 8192 | ./demod_double | sox -c 1 -r 3400 -e floating-point -b 64 -t raw - -r 19200 -t raw $tmp sinc -800  -n 8192
./uart $tmp  > $tmp.timedoctets
wc -l $tmp.timedoctets
OUT_DIR=$tmp.outdir/
mkdir -p $OUT_DIR
./split_to_to_files $OUT_DIR < $tmp.timedoctets
if [ -e ../../btx_decoder/to_image/cept_to_image ]
then
	for f in $OUT_DIR/*.cpt
	do
		../../btx_decoder/to_image/cept_to_image  $f $f.pnm
		convert $f.pnm $f.png
		rm $f.pnm
	done
fi

mv $tmp.timedoctets $tmp.outdir/timed_octets.txt

zip -j -n .png "$IN".zip $tmp.outdir/*
echo $tmp.outdir/
rm -r $tmp.outdir/
rm $tmp
