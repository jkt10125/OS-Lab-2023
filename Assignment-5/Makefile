main: main.o guest.o cleaner.o
	g++ -Wall -o main main.o guest.o cleaner.o

main.o: main.c definitions.h
guest.o: guest.c definitions.h
cleaner.o: cleaner.c definitions.h

clean:
	rm -f *.o main