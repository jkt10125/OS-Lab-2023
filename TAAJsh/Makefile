taaj: taaj.o utils.o command.o pipeline.o history.o signal_handler.o flock.o malware_detection.o
	g++ -Wall taaj.o utils.o command.o pipeline.o signal_handler.o history.o  flock.o malware_detection.o -o taaj -lreadline
taaj.o: taaj.cpp utils.hpp pipeline.hpp history.hpp signal_handler.hpp flock.hpp malware_detection.hpp
utils.o: utils.cpp utils.hpp history.hpp
command.o: command.cpp command.hpp utils.hpp
pipeline.o: pipeline.cpp pipeline.hpp command.hpp utils.hpp signal_handler.hpp
history.o: history.cpp history.hpp
signal_handler.o: signal_handler.cpp signal_handler.hpp 
flock.o: flock.cpp flock.hpp utils.hpp
malware_detection.o: malware_detection.cpp malware_detection.hpp
locker: flock_check.cpp
	g++ -Wall flock_check.cpp -o locker
malware: malware_external.cpp
	g++ -Wall malware_external.cpp -o malware
clean: 
	rm -f *.o taaj locker malware test.txt