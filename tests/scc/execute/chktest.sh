#!/bin/sh

file=${1?' empty input file'}
ttyflags=`stty -g`
trap "stty $ttyflags;tabs -8;rm -f a.out" 0 1 2 3 15
stty tabs
tabs 40
ulimit -c 0
rm -f test.log

SYS=`uname | tr A-Z a-z`
FORMAT=elf
ABI=sysv
ARCH=amd64

export SYS FORMAT ABI ARCH

while read i state
do
	echo $i >>test.log
	printf "%s\t" $i
	printf "%s" $state
	rm -f a.out

	(scc -Isysinclude $CFLAGS "$i" && ./a.out) 2>>test.log &&
		echo "[OK]" || echo "[FAILED]"
done < $file
