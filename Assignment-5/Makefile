main: main.o guest.o cleaner.o
	g++ -Wall -o main main.o guest.o cleaner.o

main.o: main.cpp definitions.h
guest.o: guest.cpp definitions.h
cleaner.o: cleaner.cpp definitions.h

clean:
	rm -f *.o main