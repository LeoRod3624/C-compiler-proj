#!/bin/bash

GCC=gcc
TMP=tmp
RUN_LINE="./$TMP"
TEST_FUNCTIONS=c_test_functions.c

assert() {
    expected="$1"
    input="$2"

    ./leocc "$input" > $TMP.s || exit
    $GCC -static -o $TMP $TMP.s $TEST_FUNCTIONS
    $RUN_LINE
    actual="$?"

    if [ "$actual" = "$expected" ]; then 
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1 
    fi
    echo "==========================================================================="
}
# assert 10 'int main() { return +10; }'
# assert 10 'int main() { return -10 + 20; }'
# assert 10 'int main() { return - -10; }'
# assert 10 'int main() { return - - +10; }'

# assert 2 'int main() { return (0 <= 1) + (1 > 0); }'

# assert 1 'int main() { return 0 < 1; }'
# assert 0 'int main() { return 1 < 1; }'
# assert 0 'int main() { return 2 < 1; }'

# assert 1 'int main() { return 0 <= 1; }'
# assert 1 'int main() { return 1 <= 1; }'
# assert 0 'int main() { return 2 <= 1; }'

# assert 1 'int main() { return 1 > 0; }'
# assert 0 'int main() { return 1 > 1; }'
# assert 0 'int main() { return 1 > 2; }'

# assert 1 'int main() { return 1 >= 0; }'
# assert 1 'int main() { return 1 >= 1; }'
# assert 0 'int main() { return 1 >= 2; }'

# assert 0 'int main() { return 0 == 1; }'
# assert 1 'int main() { return 42 == 42; }'
# assert 1 'int main() { return 0 != 1; }'
# assert 0 'int main() { return 42 != 42; }'

# assert 3 'int main() { int a; a = 1; a = 2; return 3; }'
# assert 1 'int main() { return (42 != 42) < (42 == 42); }'

# assert 3 'int main() { int a; a = 3; return a; }'




# assert 8 'int main() { int a, z; a = 3; z = 5; return a + z; }'
# assert 6 'int main() { int a, b; a = b = 3; return a + b; }'

# assert 3 'int main() { int foo, bar; foo = 3; bar = 4; return foo; }'
# assert 8 'int main() { int foo123, bar; foo123 = 3; bar = 5; return foo123 + bar; }'

# assert 42 'int main() { int a, u; u = 42; return u; }'

# assert 1 'int main() { return 1; return 2; return 3; }'
# assert 2 'int main() { int x; x = 1; return 2; x = 3; }'
# assert 3 'int main() { int x; x = 1; { int y; y = 2; return 3; } }'

# assert 5 'int main() { ;;; return 5; }'

# assert 10 'int main() { int i; i = 0; while (i < 10) { i = i + 1; } return i; }'

# assert 55 'int main() { int i, j; i = 0; j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j; }'

# assert 3 'int main() { int x; x = 3; return *&x; }'
# assert 3 'int main() { int x, *y, **z; x = 3; y = &x; z = &y; return **z; }'
# assert 5 'int main() { int x, y; x = 3; y = 5; return *(&x + 1); }'
# assert 7 'int main() { int x, y, z; x = 3; y = 5; z = 7; return *(&y + 1); }'
# assert 3 'int main() { int x, y; x = 3; y = 5; return *(&y - 1); }'
# assert 5 'int main() { int x, *y; x = 3; y = &x; *y = 5; return x; }'
# assert 7 'int main() { int x, y; x = 3; y = 5; *(&x + 1) = 7; return y; }'
# assert 7 'int main() { int x, y; x = 3; y = 5; *(&y - 1) = 7; return x; }'

# assert 3 'int main() { int x; x = 2; return x + 1; }'
# assert 2 'int main() { int x, y, *z; x = 1; y = 2; z = &x; return *(z + 1); }'
# assert 5 'int main() { int x, *y; x = 3; y = &x; *y = 5; return x; }'

# assert 1 'int main() { int a, b; a = 1; b = 2; return &b - &a; }'
# assert 5 'int main() { int x; x = 3; return &x + 2 - &x + 3; }'

# assert 2 'int main() { int x, y, *z; x = 1; y = 2; z = &x; return *(1 + z); }'

# assert 10 'int main() { int a; a = 10; return a; }'
# assert 0 'int main() { int x; x = 0; return x; }'
# assert 42 'int main() { int x; x = 42; return x; }'
# assert 5 'int main() { int a, b; b = 5; return b; }'
# assert 10 'int main() { int *p, x; x = 10; p = &x; return *p; }'

# assert 42 'int main() { return ret42(); }'
# assert 0 'int main() { return ret0(); }'
# assert 42 'int main() { int x; x = ret42(); return x; }'
# assert 0 'int main() { int x; x = ret0(); return x; }'
# assert 0 'int main() { while (ret0()) return 42; return 0; }'
# assert 42 'int main() { { return ret42(); } }'
# assert 0 'int main() { return ret0() * ret42(); }'
# assert 0 'int main() { return ret0() / ret42(); }'
# assert 5 'int main() { int a = 5, b = 1; return a * b; }'
# assert 6 'int main() { int a = 5, b = 1; a = 5 + 1; return a; }'
# assert 42 'int main() { int a = 5, b = ret42(); b = ret42() + 0; return b; }'
# assert 5 'int main() { int a = 5; return a; }'
# assert 8 'int main() { int a = 0, b = 5, c = 3; return b + 3; }'
# assert 42 'int main() { int a, b; a = ret42(); b = ret0(); return a + b; }'
# assert 52 'int main() { int x, y; x = 10; y = ret42(); return x + y; }'
# assert 42 'int main() { return ret42() + ret0(); }'
# assert 42 'int main() { return ret42() - ret0(); }'

# assert 16 'int main() { int* a = 8; return a + 1; }'

# assert 9 'int main() { return add_two_numbers(4, 5); }'
# assert 0 'int main() { return add_two_numbers(255, 1); }'
# assert 4 'int main() { return add_two_numbers(add_two_numbers(1,1), 1+1); }' 
# assert 21 'int main() { return add6(1,2,3,4,5,6); }'
# assert 3 'int main() { return subtract_two_numbers(5, 2); }'
# assert 12 'int main() { return subtract_two_numbers(8, -4); }'
# assert 8 'int main() { return add(3, 5); }'
# assert 10 'int main() { return add(2 + 3, 5); }'
# assert 7 'int main() { int x = add(2,2); return add(x,3); }'
# assert 21 'int main() { return add6(1,2,3,4,5,6); }'
# assert 11 'int main() { return inc2(inc2(7)); }'
# assert 2 'int main() { return sub(5, 3); }'
# assert 66 'int main() { return add6(1,2,add6(3,4,5,6,7,8),9,10,11); }'
# assert 137 'int main() { return add6(1,2,add6(3,add6(4,5,6,7,8,9),10,11,12,13),14,15,16+1); }'

# assert 42 "int main() { return 42; }"
# assert 10 "int ret10() { return 10; } int main() { return ret10(); }"
# assert 0 "int main() { return 0; }"



# assert 15 "int ADD(int a, int b) { return a + b; } int main() { return ADD(10, 5); }"
# assert 21 "int ADD6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; } int main() { return ADD6(1, 2, 3, 4, 5, 6); }"
# assert 42 "int identity(int x) { return x; } int main() { return identity(42); }"
# assert 5 "int f() { int x = 5; return x; } int g() { int x = 10; return x; } int main() { return f(); }"
# assert 15 "int f() { int x = 5; return x; } int g() { int x = 10; return x; } int main() { return f() + g(); }"


# assert 25 "int f() { int x = 5; return x; } int g() { int x = 10; return x; } int main() { int x = 10; return x + f() + g(); }"


# assert 3 'int main() { int x, *y, **z; x = 3; y = &x; z = &y; return **z; }'
# assert 3 'int main() { int x ;int* y;x = 3;y = &x;return *y;}'
 
# assert 42 'int main() {int x, *y, **z, ***w;x = 42;y = &x;z = &y;w = &z;return ***w;}'
# assert 10 'int deref(int* p) { return *p; } int main() { int x = 10; return deref(&x); }'
 
#  # 9. Return from function with 4-level pointer param
# assert 99 'int f(int**** pppp) { return ****pppp; } int main() { int a = 99, *b = &a, **c = &b, ***d = &c; return f(&d); }'
 
#  # 10. Modify value through triple pointer and return it
# assert 100 'int main() { int a, *b, **c, ***d; b = &a; c = &b; d = &c; ***d = 100; return a; }'
 
#  #11. Function adds dereferenced pointer values
# assert 111 'int f(int* x, int* y) { return *x + *y;} int main() { int a = 50, b = 61; return f(&a, &b); }'
 
#  # # 12. Pointer math inside function
# assert 130 'int f(int* p) { return *(p + 1); } int main() { int x = 5, y = 130; return f(&x); }'
#==============================================================================================================

# assert 5 'int main() { return 2 + 3; }'

# assert 5 'int main() { int x = 5; return x; }'
# assert 12 'int main() { int x = 5; int y = 7; return x + y; }'
# assert 3 'int main() { int x = 0; while (x < 3) { x = x + 1; } return x; }'
# assert 5 'int main() { int x = 0; for (x = 0; x < 5; x = x + 1) {} return x; }'

# assert 1 'int main() { return 2 < 3; }'
# assert 0 'int main() { return 3 < 2; }'
# assert 1 'int main() { return 4 > 2; }'
# assert 0 'int main() { return 2 > 4; }'
# assert 1 'int main() { return 5 <= 5; }'
# assert 1 'int main() { return 4 <= 5; }'
# assert 0 'int main() { return 6 <= 5; }'
# assert 1 'int main() { return 5 >= 5; }'
# assert 1 'int main() { return 6 >= 5; }'
# assert 0 'int main() { return 4 >= 5; }'
# assert 1 'int main() { return 3 == 3; }'
# assert 0 'int main() { return 3 == 4; }'
# assert 1 'int main() { return 3 != 4; }'
# assert 0 'int main() { return 3 != 3; }'

# assert 1 'int main() { if (1) {return 1;} else {return 2;} }'
# assert 2 'int main() { if (0) {return 1;} else {return 2;} }'
# assert 3 'int main() { int x = 1; if (x == 1) {x = 3;} return x; }'
# assert 4 'int main() { int x = 1; if (x != 1) {x = 2;} else {x = 4;} return x; }'
# assert 5 'int main() { int x = 5; if (x > 0) { x = x + 1; } return x - 1; }'
# assert 7 'int main() { int x = 2; if (x < 3) { x = 7; } else { x = 9; } return x; }'
# assert 7 'int main() { int x = 1; for (x = 1; x < 3; x = x + 1) { x = x + 1; } return x + 4; }'
# assert 3 'int main() { int i = 0; while (i < 3) { i = i + 1; } return i; }'

# assert 5 'int main() { int x = 0; while (x < 5) { if (x == 3) { x = x + 2; } else { x = x + 1; } } return x; }'
# assert 3 'int main() { int x = 0; if (1>0) { while (x < 3) { x = x + 1; } } return x; }'
# assert 10 'int main() { int x = 0; int i = 0; if (1) { for (i = 0; i < 5; i = i + 1) { x = x + 2; } } return x; }'
# assert 2 'int main() { int z = 4; int sum = 0; int i = 0; for (i = 0; i < z; i = i + 1) { if (i == 0) { sum = sum + i;} if (i == 2) { sum = sum + i; } } return sum; }'

# assert 1 'int main() { return 2 - 1; }'
# assert 12 'int main() { return 3 * 4; }'
# assert 4 'int main() { return 8 / 2; }'
# assert 4 'int main() { int x = 6; int y = 2; return x - y; }'
# assert 18 'int main() { int x = 6; int y = 3; return x * y; }'
# assert 5 'int main() { int x = 10; int y = 2; return x / y; }'
# assert 24 'int main() { int x = 5; int y = 2; return (x + 3) * (y + 1); }'
# assert 2 'int main() { int x = 8; return x / (2 + 2); }'
# assert 0 'int main() { return 5 - 5; }'

assert 5 'int main() { int x = 5; return *(&x); }'

assert 42 'int main() { if (1) { return 42; } return 0; }'
assert 0 'int main() { if (0) { return 42; } return 0; }'
assert 1 'int main() { if (1) { return 1; } else { return 2; } }'
assert 2 'int main() { if (0) { return 1; } else { return 2; } }'
assert 7 'int main() { int x = 3; if (x == 3) { return 7; } return 0; }'
assert 5 'int main() { int x = 4; if (x == 3) { return 7; } return 5; }'

echo OK