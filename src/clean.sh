#!/bin/bash

for x in *.c
do
	y=`basename $x .c`
	rm $y
done
