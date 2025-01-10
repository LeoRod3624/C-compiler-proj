#!/bin/bash

# Set defaults to x86
GCC=gcc
TMP=tmp
RUN_LINE="./$TMP"
TEST_FUNCTIONS=test_functions.s  # External assembly file for function definitions
TEST_FUNCTIONS2=c_test_functions.c # External c file for function definitions

assert() {
    expected="$1"
    input="$2"

    # Compile the input to assembly
    ./leocc "$input" > $TMP.s || exit

    # Link with the external assembly file for function definitions
    $GCC -static -o $TMP $TMP.s $TEST_FUNCTIONS $TEST_FUNCTIONS2

    # Run the resulting executable
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

assert 42 '{ return ret42(); }'
assert 0 '{ return ret0(); }'
assert 42 '{ int x; x = ret42(); return x; }'
assert 0 '{ int x; x = ret0(); return x; }'
assert 0 '{ while (ret0()) return 42; return 0; }'
assert 42 '{ { return ret42(); } }'
assert 0 '{ return ret0() * ret42(); }'
assert 0 '{ return ret0() / ret42(); }'
assert 5 '{ int a = 5, b = 1; return a * b; }'
assert 6 '{ int a = 5, b = 1; a = 5 + 1; return a; }'
assert 42 '{ int a = 5, b = ret42(); b = ret42() + 0; return b; }'
assert 5 '{ int a = 5; return a; }'
assert 8 '{ int a = 0, b = 5, c = 3; return b + 3; }'
assert 42 '{ int a, b; a = ret42(); b = ret0(); return a + b; }'
assert 52 '{ int x, y; x = 10; y = ret42(); return x + y; }'
assert 42 '{ return ret42() + ret0(); }'
assert 42 '{ return ret42() - ret0(); }'

assert 16 '{ int* a = 8; return a + 1;}'

assert 9 '{ return add_two_numbers(4, 5); }' #assembly style tests
assert 0 '{ return add_two_numbers(255, 1); }' #This makes sense because we can only have vals 0-255, so 256 is just the next set of 'bits'.
assert 4 '{ return add_two_numbers(add_two_numbers(1,1), 1+1); }' 
assert 21 '{ return add6(1,2,3,4,5,6); }'
assert 3 '{ return subtract_two_numbers(5, 2); }'
assert 12 '{ return subtract_two_numbers(8, -4); }'
#Both tests are linked to my assembly file.
assert 8 '{ return add(3, 5); }' #c style function tests
assert 10 '{ return add(2 + 3, 5); }'
assert 7 '{ int x = add(2,2); return add(x,3); }'
assert 21 '{ return add6(1,2,3,4,5,6); }'
assert 11 '{ return inc2(inc2(7)); }'
assert 2 '{ return sub(5, 3); }'
assert 66 '{ return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
assert 137 '{ return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16+1); }'

echo OK
