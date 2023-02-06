taaj: taaj.o utils.o command.o pipeline.o
	g++ -Wall taaj.o utils.o command.o pipeline.o -o taaj
taaj.o: taaj.cpp utils.hpp pipeline.hpp 
utils.o: utils.cpp utils.hpp
command.o: command.cpp command.hpp utils.hpp
pipeline.o: pipeline.cpp pipeline.hpp command.hpp utils.hpp
