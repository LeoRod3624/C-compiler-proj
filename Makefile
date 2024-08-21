CC=clang++
CFLAGS=-std=c++11 -g -fno-common
leocc: main.cpp main.hpp cst.o
#	$(CC) -c cst.hpp $(CFLAGS)
	$(CC) -o leocc cst.o main.cpp $(CFLAGS)




test: leocc
	./test.sh 

clean:
	rm -f leocc *.o *~ tmp* *.s

.PHONY: test clean 
