sns: sns.o userSimulator.o pushUpdate.o readPost.o
	g++ -Wall sns.o userSimulator.o pushUpdate.o readPost.o -lpthread -o sns
sns.o: sns.cpp
userSimulator.o: userSimulator.cpp
pushUpdate.o: pushUpdate.cpp
readPost.o: readPost.cpp

clean:
	rm *.o sns
