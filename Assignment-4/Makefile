sns: sns.o userSimulator.o pushUpdate.o readPost.o definitions.o
	g++ -Wall sns.o definitions.o userSimulator.o pushUpdate.o readPost.o -lpthread -o sns
sns.o: sns.cpp definitions.hpp
userSimulator.o: userSimulator.cpp definitions.hpp
pushUpdate.o: pushUpdate.cpp definitions.hpp
readPost.o: readPost.cpp definitions.hpp
definitions.o: definitions.cpp definitions.hpp
clean:
	rm *.o sns sns.log users.log time.log
