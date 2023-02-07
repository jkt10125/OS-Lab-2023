taaj: taaj.o utils.o command.o pipeline.o history.o
	g++ -Wall taaj.o utils.o command.o pipeline.o  history.o -o taaj
taaj.o: taaj.cpp utils.hpp pipeline.hpp history.hpp
utils.o: utils.cpp utils.hpp history.hpp
command.o: command.cpp command.hpp utils.hpp
pipeline.o: pipeline.cpp pipeline.hpp command.hpp utils.hpp
history.o: history.cpp history.hpp
clean: 
	rm -f *.o taaj