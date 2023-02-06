taaj: taaj.o utils.o command.o
	g++ -Wall taaj.o utils.o command.o -o taaj
taaj.o: taaj.cpp utils.hpp command.hpp
utils.o: utils.cpp utils.hpp
command.o: command.cpp command.hpp utils.hpp
