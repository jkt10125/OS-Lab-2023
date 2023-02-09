taaj: taaj.o utils.o command.o pipeline.o history.o signal_handler.o flock.o
	g++ -Wall taaj.o utils.o command.o pipeline.o signal_handler.o history.o  flock.o -o taaj -lreadline
taaj.o: taaj.cpp utils.hpp pipeline.hpp history.hpp signal_handler.hpp flock.hpp
utils.o: utils.cpp utils.hpp history.hpp
command.o: command.cpp command.hpp utils.hpp
pipeline.o: pipeline.cpp pipeline.hpp command.hpp utils.hpp signal_handler.hpp
history.o: history.cpp history.hpp
signal_handler.o: signal_handler.cpp signal_handler.hpp 
flock.o: flock.cpp flock.hpp utils.hpp
clean: 
	rm -f *.o taaj