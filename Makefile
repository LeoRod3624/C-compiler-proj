CC=clang++
WARNINGS=''
CFLAGS=-g -fno-common $(WARNINGS)
leocc: main.cpp leocc.hpp cst.cpp ast.cpp codegen.cpp 
	$(CC) -o leocc *.cpp $(CFLAGS) -I /usr/include/llvm-18 -I /usr/include/llvm-c-18  -lLLVM-18




test: leocc
	./test.sh 

clean:
	rm -f leocc *.o *~ tmp* *.s

.PHONY: test clean 
