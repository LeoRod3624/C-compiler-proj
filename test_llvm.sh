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
echo OK
