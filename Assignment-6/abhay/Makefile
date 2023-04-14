
all: libgoodmalloc.a test

libgoodmalloc.a: goodmalloc.o
	@ echo -n "Building library... "
	@ ar rcs -o libgoodmalloc.a goodmalloc.o
	@ echo "Done"

goodmalloc.o: goodmalloc.cpp goodmalloc.hpp
	@ g++ -Wall -c -o goodmalloc.o goodmalloc.cpp

test: mergesort.cpp libgoodmalloc.a
	@ echo -n "Compiling test file... "
	@ g++ -Wall -o test mergesort.cpp -L. -lgoodmalloc
	@ echo "Done"
	@ echo -n "Running test file... "
	@ ./test > out.txt 
	@ echo "Done"

install: libgoodmalloc.a
	@ echo -n "Installing library... "
	@ cp -p goodmalloc.hpp /usr/local/include
	@ cp -p libgoodmalloc.a /usr/local/lib
	@ echo "Done"
	@ echo -n "Cleaning directory... "
	@ rm -f libgoodmalloc.a goodmalloc.o test
	@ echo "Done"
	
clean:
	@ echo -n "Cleaning directory... "
	@ rm -f libgoodmalloc.a goodmalloc.o test out.txt output.txt
	@ echo "Done"