#!/bin/bash

#set defaults to x86
GCC=x86_64-linux-gnu-gcc
TMP=tmp
RUN_LINE="qemu-x86_64-static ./$TMP"

assert() {
    expected="$1"
    input="$2"

    ./leocc "$input" > $TMP.s || exit
    $GCC -static -o $TMP $TMP.s 
    $RUN_LINE
    actual="$?"

    if [ "$actual" = "$expected" ]; then 
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1 
    fi
}

assert 0 0 
assert 42 42 
assert 7 '((7))'
#vvvvNext goalvvvv
# assert 47 '41+6'

# assert 15 '5+20-10'
# assert 15 '5 +20    -    10     ' 

echo OK
