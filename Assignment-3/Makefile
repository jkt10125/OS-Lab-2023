main: main.o producer.o consumer.o
	g++ -Wall main.o producer.o consumer.o -o main
main.o: main.cpp definitions.h
producer.o: producer.cpp definitions.h
consumer.o: consumer.cpp definitions.h
clean:
	rm -f *.o main consumer*.txt