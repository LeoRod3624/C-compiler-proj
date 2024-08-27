CC=clang++
CFLAGS=-std=c++11 -g -fno-common
leocc: main.cpp leocc.hpp cst.cpp ast.cpp codegen.cpp
	$(CC) -o leocc *.cpp $(CFLAGS)




test: leocc
	./test.sh 

clean:
	rm -f leocc *.o *~ tmp* *.s

.PHONY: test clean 
