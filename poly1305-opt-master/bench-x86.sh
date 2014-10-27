#!/bin/bash

usage() {
	echo "usage:   ./bench-x86.sh [ref8,ref32,x86,sse2,avx,avx2] [32,64]"
	echo ""
}

bench() {
	local FN
	local IMPL=$1
	case $IMPL in
		ref8) FN=extensions/poly1305_ref-8.c
		      IMPL="ref";;
		ref32) FN=extensions/poly1305_ref-32.c
		       IMPL="ref";;
		*) FN=extensions/poly1305_$IMPL-$2.S;;
	esac

	if [ ! -f $FN ] ; then
		echo $FN " doesn't exist to bench!"
		exit 1;
	fi

	echo "benching "$1", "$2" bits, single implementation"
	gcc poly1305.c bench-x86.c -DPOLY1305_IMPL=$IMPL $FN -O3 -o poly1305_bench_$IMPL -m$2 2>/dev/null
	local RC=$?
	if [ $RC -ne 0 ]; then
		echo $FN " failed to compile"
		return
	fi
	./poly1305_bench_$IMPL
	rm -f poly1305_bench_$IMPL
}

case $2 in
	32);;
	64);;
	"") usage; exit 1;;
	*) usage; echo "arch must be 32 or 64 bits!"; exit 1;;
esac

bench $1 $2
