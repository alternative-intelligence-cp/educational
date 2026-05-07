#!/usr/bin/env bash

wdir="$(dirname "$(readlink -f "$0")")"

clear &&

gcc -c -o "$wdir/test.o" "$wdir/test.c" && 
gcc -c -o "$wdir/ring_buffer.o" "$wdir/ring_buffer.c" &&
gcc -c -o "$wdir/ring_buffer_errors.o" "$wdir/ring_buffer_errors.c" &&
gcc -c -o "$wdir/ring_buffer_ulong.o" "$wdir/ring_buffer_ulong.c" &&
gcc -c -o "$wdir/ring_buffer_double.o" "ring_buffer_double.c" &&
 
gcc -o "$wdir/test" "$wdir/test.o" "$wdir/ring_buffer.o" "$wdir/ring_buffer_errors.o" "$wdir/ring_buffer_ulong.o" "$wdir/ring_buffer_double.o" &&

"$wdir/test"

rm -f "$wdir/"*.o
rm -f "$wdir/test"
