
all: libmemlab.a test

libmemlab.a: memlab.o
	@ echo -n "Building library... "
	@ ar rcs -o libmemlab.a memlab.o
	@ echo "Done"

memlab.o: memlab.cpp memlab.hpp
	@ g++ -Wall -c -o memlab.o memlab.cpp

test: mergesort.cpp libmemlab.a
	@ echo -n "Compiling test file... "
	@ g++ -Wall -o test mergesort.cpp -L. -lmemlab
	@ echo "Done"

install: libmemlab.a
	@ echo -n "Installing library... "
	@ cp -p memlab.hpp /usr/local/include
	@ cp -p libmemlab.a /usr/local/lib
	@ echo "Done"
	@ echo -n "Cleaning directory... "
	@ rm -f libmemlab.a memlab.o test
	@ echo "Done"
	
clean:
	@ echo -n "Cleaning directory... "
	@ rm -f libmemlab.a memlab.o test
	@ echo "Done"