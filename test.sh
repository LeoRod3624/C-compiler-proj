#!/bin/bash

#set defaults to x86
GCC=gcc
TMP=tmp
RUN_LINE="./$TMP"

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
assert 1 '(1)'
assert 7 '2+5'
assert 1 '4-3'
assert 8 '4*2'
assert 4 '8/2'
assert 30 '((((5+5)+(8/2)/(2)*(10))))'

#vvvvNext goalvvvv
# assert 47 '41+6'

# assert 15 '5+20-10'
# assert 15 '5 +20    -    10     ' 

echo OK
