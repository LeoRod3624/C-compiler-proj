CC=clang++
WARNINGS=''
CFLAGS=-g -fno-common $(WARNINGS)

leocc: main.cpp leocc.hpp cst.cpp ast.cpp codegen.cpp
	$(CC) -o leocc *.cpp $(CFLAGS) -I /usr/include/llvm-14 -I /usr/include/llvm-c-14 -lLLVM-14

# ARM backend tests
test: leocc
	LEO_BACKEND= ./test.sh

# LLVM IR backend tests
test_llvm: leocc test_llvm.sh
	chmod +x test_llvm.sh
	LEO_BACKEND=llvm ./test_llvm.sh

clean:
	rm -f leocc *.o *~ tmp* *.s output.ll

.PHONY: test test_llvm clean
