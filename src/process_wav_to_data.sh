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
sox "$IN" -c 1 -r 6800 -e signed-integer -b 16 -t raw - sinc 10 -n 8192 | ./real_to_iq_16bit | sox -c 2 -r 3400 -e signed-integer -b 16 -t raw - -t raw - sinc -800 | ./demod_16bit | sox -c 1 -r 3400 -e signed-integer -b 16 -t raw - -r 19200 -t raw $tmp
./uart_16bit $tmp  > $tmp.timedoctets 2> $tmp.errors

wc -l $tmp.timedoctets
OUT_DIR=$tmp.outdir/
mkdir -p $OUT_DIR
./split_to_to_files $OUT_DIR < $tmp.timedoctets
if [ -e ../../btx_decoder/to_image/cept_to_image ]
then
	rm $tmp.cpt
	for f in $OUT_DIR/*.cpt
	do
		cat $f >> $tmp.cpt
		../../btx_decoder/to_image/cept_to_image  $tmp.cpt $f.pnm
		convert $f.pnm $f.png
		rm $f.pnm
	done
fi

mv $tmp.timedoctets $tmp.outdir/timed_octets.txt

zip -j -n .png "$IN".zip $tmp.outdir/*
echo $tmp.outdir/
#rm -r $tmp.outdir/
rm $tmp
