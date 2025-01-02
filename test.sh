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

assert 10 '{ return +10; }'
assert 10 '{ return -10 + 20; }'
assert 10 '{ return - -10; }'
assert 10 '{ return - - +10; }'

assert 2 '{ return (0 <= 1) + (1 > 0); }'

assert 1 '{ return 0 < 1; }'
assert 0 '{ return 1 < 1; }'
assert 0 '{ return 2 < 1; }'

assert 1 '{ return 0 <= 1; }'
assert 1 '{ return 1 <= 1; }'
assert 0 '{ return 2 <= 1; }'

assert 1 '{ return 1 > 0; }'
assert 0 '{ return 1 > 1; }'
assert 0 '{ return 1 > 2; }'

assert 1 '{ return 1 >= 0; }'
assert 1 '{ return 1 >= 1; }'
assert 0 '{ return 1 >= 2; }'

assert 0 '{ return 0 == 1; }'
assert 1 '{ return 42 == 42; }'
assert 1 '{ return 0 != 1; }'
assert 0 '{ return 42 != 42; }'

assert 1 '{ return (42 != 42) < (42 == 42); }'
assert 3 '{ int a; a = 1; a = 2; return 3; }'

assert 3 '{ int a; a = 3; return a; }'
assert 8 '{ int a, z; a = 3; z = 5; return a + z; }'
assert 6 '{ int a, b; a = b = 3; return a + b; }'

assert 3 '{ int foo, bar; foo = 3; bar = 4; return foo; }'
assert 8 '{ int foo123, bar; foo123 = 3; bar = 5; return foo123 + bar; }'

assert 42 '{ int a, u; u = 42; return u; }'

assert 1 '{ return 1; return 2; return 3; }'
assert 2 '{ int x; x = 1; return 2; x = 3; }'
assert 3 '{ int x; x = 1; { int y; y = 2; return 3; } }'

assert 5 '{ ;;; return 5; }'

assert 10 '{ int i; i = 0; while (i < 10) { i = i + 1; } return i; }'

assert 55 '{ int i, j; i = 0; j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j; }'

assert 3 '{ int x; x = 3; return *&x; }'
assert 3 '{ int x, *y, **z; x = 3; y = &x; z = &y; return **z; }'
assert 5 '{ int x, y; x = 3; y = 5; return *(&x + 1); }'
assert 7 '{ int x, y, z; x = 3; y = 5; z = 7; return *(&y + 1); }'
assert 3 '{ int x, y; x = 3; y = 5; return *(&y - 1); }'
assert 5 '{ int x, *y; x = 3; y = &x; *y = 5; return x; }'
assert 7 '{ int x, y; x = 3; y = 5; *(&x + 1) = 7; return y; }'
assert 7 '{ int x, y; x = 3; y = 5; *(&y - 1) = 7; return x; }'

assert 3 '{ int x; x = 2; return x + 1; }'
assert 2 '{ int x, y, *z; x = 1; y = 2; z = &x; return *(z + 1); }'
assert 5 '{ int x, *y; x = 3; y = &x; *y = 5; return x; }'

assert 1 '{ int a, b; a = 1; b = 2; return &b - &a; }'
assert 5 '{ int x; x = 3; return &x + 2 - &x + 3; }'

assert 2 '{ int x, y, *z; x = 1; y = 2; z = &x; return *(1 + z); }'

assert 10 '{ int a; a = 10; return a; }'
assert 0 '{ int x; x = 0; return x; }'
assert 42 '{ int x; x = 42; return x; }'
assert 5 '{ int a, b; b = 5; return b; }'
assert 10 '{ int *p, x; x = 10; p = &x; return *p; }'

echo OK
