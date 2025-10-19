#!/bin/bash
set -u  # (no -e, so we can capture non-zero exit codes)

GCC=clang
TMP=tmp
RUN_LINE="./$TMP"
TEST_FUNCTIONS=c_test_functions.c  # your helper C file

assert() {
  expected="$1"
  input="$2"

  # 1) Generate LLVM IR
  ./leocc "$input" || exit 1

  # 2) Build executable from IR + helpers (no -static)
  $GCC -O2 -o "$TMP" output.ll "$TEST_FUNCTIONS" || exit 1

  # 3) Run and capture exit code (even if non-zero)
  "$RUN_LINE"
  actual=$?

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
echo OK
