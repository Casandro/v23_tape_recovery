#!/bin/bash

set -e

bail () {
	echo $*
	exit;
}


for x in *.c
do
	gcc -O3 -Wall -lm $x -o `basename $x .c` || bail "Fehler bei $x"
done


IN="$1"
OUT=/daten/bildschirmtext/tapes_mspt_2024/data/`basename "$IN" .flac`

echo $OUT

FORMAT="-e signed-integer -b 16 -t raw"

tmp=`mktemp /tmp/v23tapeXXXXXX`
echo $tmp
sox -v 0.8 "$IN" -c 1 -r 6800 $FORMAT  - norm -6 sinc 800-3400 -n 512 | ./real_to_iq_16bit | sox -c 2 -r 3400 $FORMAT - -t raw -  | ./demod_16bit | sox -c 1 -r 3400 $FORMAT - -r 19200 -t raw $tmp
./uart_16bit $tmp  | ./find_files "$OUT-" > "$OUT.index"


rm $tmp
