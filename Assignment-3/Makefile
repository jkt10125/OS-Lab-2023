main: main.o producer.o consumer.o
	g++ -Wall main.o producer.o consumer.o -o main
main.o: main.cpp definitions.h consumer.h producer.h
producer.o: producer.cpp producer.h definitions.h
consumer.o: consumer.cpp consumer.h definitions.h
clean:
	rm *.o main