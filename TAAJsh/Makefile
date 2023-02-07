taaj: taaj.o utils.o command.o pipeline.o history.o signal_handler.o
	g++ -Wall taaj.o utils.o command.o pipeline.o signal_handler.o history.o -o taaj
taaj.o: taaj.cpp utils.hpp pipeline.hpp history.hpp signal_handler.hpp
utils.o: utils.cpp utils.hpp history.hpp
command.o: command.cpp command.hpp utils.hpp
pipeline.o: pipeline.cpp pipeline.hpp command.hpp utils.hpp signal_handler.hpp
history.o: history.cpp history.hpp
signal_handler.o: signal_handler.cpp signal_handler.hpp 
clean: 
	rm -f *.o taaj