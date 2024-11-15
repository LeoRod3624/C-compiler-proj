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

assert 0 '{ return 0; }' 
assert 42 '{return 42;}' 
assert 1 '{return (1);}'
assert 7 '{return 2+5;}'
assert 1 '{return 4-3;}'
assert 8 '{return 4*2;}'
assert 4 '{return 8/2;}'
assert 30 '{return ((((5+5)+(8/2)/(2)*(10))));}'

assert 10 '{return (5)*(2);}'

assert 10 '{return +10;}'
assert 10 '{return -10+20;}'
assert 10 '{return - -10;}'
assert 10 '{return - - +10;}'

assert 2 '{return (0<=1) + (1>0);}'

assert 1 '{return 0<1;}'
assert 0 '{return 1<1;}'
assert 0 '{return 2<1;}'

assert 1 '{return 0<=1;}'
assert 1 '{return 1<=1;}'
assert 0 '{return 2<=1;}'

assert 1 '{return 1>0;}'
assert 0 '{return 1>1;}'
assert 0 '{return 1>2;}'

assert 1 '{return 1>=0;}'
assert 1 '{return 1>=1;}'
assert 0 '{return 1>=2;}'

assert 0 '{return 0==1;'}
assert 1 '{return 42==42;}'
assert 1 '{return 0!=1;'}
assert 0 '{return 42!=42;}'

assert 1 '{return (42!=42) < (42==42);}'
assert 3 '{1; 2; return 3;}'


assert 3 '{a=3; return a;}'
assert 8 '{a=3; z=5; return a+z;}'
assert 6 '{a=b=3; return a+b;}'

assert 3 '{foo=3; far=4; return foo;}' # this would fail if we only allowed single letter variables, since they start with the same letter
assert 8 '{foo123=3; bar=5; return foo123+bar;}'

assert 42 '{a=1; b;c;d;e;f;g;h;i;j;k;l;m;n;o;p;q;r;s;t;u=42;v;w;x;y;z;aa;bb;cc;dd;ee;ff; return u;}'

assert 1 '{return 1; 2; 3;}'
assert 2 '{1; return 2; 3;}'
assert 3 '{{1;} {2;} {return 3;}}'

assert 5 '{ ;;; return 5; }'


assert 10 '{ i=0; while(i<10) { i=i+1; } return i; }'

assert 55 '{ i=0; j=0; for (i=0; i<=10; i=i+1) j=i+j; return j; }'

assert 3 '{ x=3; return *&x; }'
assert 3 '{ x=3; y=&x; z=&y; return **z;}'
assert 5 '{ x=3; y=5; return *(&x+1);}'
assert 7 '{ x=3; y=5; z=7; return *(&y+1);}'
assert 3 '{ x=3; y=5; return *(&y-1); }'
assert 5 '{ x=3; y=&x; *y=5; return x; }'
assert 7 '{ x=3; y=5; *(&x+1)=7; return y; }'
assert 7 '{ x=3; y=5; *(&y-1)=7; return x; }'

assert 3 '{ x = 2; return x + 1; }'
assert 2 '{ x = 1; y = 2; z = &x; return *(z+1); }'
assert 5 '{ x=3; y=&x; *y=5; return x; }'

assert 1 '{ a=1;b=2; return &b-&a; }'
assert 5 '{ x=3; return &x+2-&x+3; }'

assert 2 '{ x = 1; y = 2; z = &x; return *(1+z); }'

echo OK
