#!/bin/bash
set -u  # (no -e, so we can capture non-zero exit codes)

GCC=clang
TMP=tmp
RUN_LINE="./$TMP"
TEST_FUNCTIONS=c_test_functions.c  # your helper C file

assert() {
    expected="$1"
    input="$2"

    # Compile IR and link (unchanged)
    ./leocc "$input" >/dev/null || exit 1
    clang -O2 output.ll -o tmp || exit 1

    # Run and capture raw exit code (0..255)
    ./tmp
    rc=$?

    # Interpret rc as signed 8-bit (so 253 -> -3, etc.)
    if [ "$rc" -ge 128 ]; then
        actual=$((rc - 256))
    else
        actual=$rc
    fi

    if [ "$actual" = "$expected" ]; then 
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1 
    fi
}

# Start with the cases your LLVM backend supports today:

assert 7 'int main(){ return 7; }'

assert 5 'int main(){ return 2 + 3; }'
assert 3 'int main(){ return 8 - 5; }'
assert 42 'int main(){ return 6 * 7; }'
assert 3 'int main(){ return 9 / 3; }'
assert 10 'int main(){ return 1 + 2 + 3 + 4; }'
assert 14 'int main(){ return 2 * 3 + 8; }'

assert 1 'int main(){ return 1 < 2; }'
assert 0 'int main(){ return 2 < 1; }'
assert 1 'int main(){ return 3 >= 3; }'
assert 0 'int main(){ return 3 >= 4; }'
assert 1 'int main(){ return 10 == 10; }'
assert 0 'int main(){ return 10 != 10; }'
assert 2 'int main(){ return (0 <= 1) + (1 > 0); }'

assert 3  'int main(){ int a; a = 3; return a; }'
assert 8  'int main(){ int a; a = 3; int b; b = 5; return a + b; }'
assert 42 'int main(){ int x = 42; return x; }'
assert 6  'int main(){ int a = 1; int b = 2; int c = 3; return a + b + c; }'

assert -3 'int main(){ return -3; }'
assert -1 'int main(){ return -2 + 1; }'
assert 1  'int main(){ return -2 < -1; }'

assert 10 'int main() { int i; i = 0; while (i < 10) { i = i + 1; } return i; }'
assert 55 'int main() { int i, j; i = 0; j = 0; for (i = 0; i <= 10; i = i + 1) j = i + j; return j; }'

assert 7 'int inc(int x) { return x + 1; } int main() { return inc(6); }'

assert 7 'int main() { int x = 5; int *p = &x; *p = 7; return x; }'

# more function tests

assert 6  'int add3(int a, int b, int c) { return a + b + c; } int main() { return add3(1, 2, 3); }'
assert 10 'int add(int a, int b) { return a + b; } int main() { return add(add(1, 2), add(3, 4)); }'
#pointer testing

assert 9  'int main() { int x = 5; int *p = &x; *p = 9; return x; }'
assert 7  'int main() { int x = 3; int y = 7; int *p = &x; *p = y; return x; }'
assert 9  'int main() { int x = 5; int *p = &x; int *q = p; *q = 9; return x; }'
assert 20 'int main() { int x = 10; int *p = &x; int **pp = &p; **pp = 20; return x; }'
assert 1  'int main() { int x = 1; int y = 2; int *p = &x; int *q = &y; int **pp = &p; *pp = q; *q = 7; return x; }'
assert 5  'int main() { int x = 1; int y = 2; int *p = &x; int *q = &y; int **pp = &p; *pp = q; *p = 5; return y; }'
assert 7  'int main() { int x = 3; int y = 4; int *p = &x; int *q = &y; *p = *p + *q; return x; }'
assert 1  'int main() { int x = 5; int *p = &x; *p = 7; return *p == x; }'
assert 10 'int main() { int x = 0; int *p = &x; int i; i = 0; while (i < 5) { *p = *p + 2; i = i + 1; } return x; }'
assert 3  'int main() { int x = 0; int *p = &x; int i; for (i = 0; i < 3; i = i + 1) { *p = *p + i; } return x; }'
assert -5 'int main() { int x = 3; int *p = &x; *p = -5; return x; }'

assert 4  'int main() { int x = 3, y = 4; int *p = &x, *q = &y; *p = *q; return x; }'

# Explicit null statements
assert 5  'int main() { int x = 5; ; return x; }'
#testing empty init
assert 3  'int main() { int i; i = 0; for (; i < 3; i = i + 1) ; return i; }'

assert 4  'int main() { int x = 0; int *p = &x; int i; for (i = 0; i < 4;) { *p = *p + 1; i = i + 1; } return x; }'

# nested for loops (no pointers)
assert 6 'int main() { int i; int j; int s; s = 0; for (i = 0; i < 3; i = i + 1) { for (j = 0; j < 2; j = j + 1) { s = s + 1; } } return s; }'

# for with missing increment; body does the increment
assert 4 'int main() { int i; i = 0; for (; i < 4; ) { i = i + 1; } return i; }'

# for with missing init; pointer used in the body
assert 6 'int main() { int x = 0; int *p = &x; int i; i = 0; for (; i < 3; i = i + 1) { *p = *p + 2; } return x; }'

# for with single-statement body (no braces) using a pointer
assert 4 'int main() { int x = 0; int *p = &x; int i; for (i = 0; i < 4; i = i + 1) *p = *p + 1; return x; }'

# double-pointer update
assert 5 'int main() { int x = 1; int *p = &x; int **pp = &p; **pp = **pp + 4; return x; }'

# assignment as an expression with a pointer lvalue
assert 7 'int main() { int x = 0; int *p = &x; return *p = 7; }'

echo OK
