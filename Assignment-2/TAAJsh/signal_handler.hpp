#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include "./pipeline.hpp"
#include<vector>
#include<map>
#include <sys/types.h>


extern pid_t fgpid;
extern const pid_t rootpid;
extern std::vector<Pipeline*> pipesArr;
extern std::map<pid_t, int> pid2index;

void reapProcesses(int sig);
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD();
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);
void ctrlChandler(int sig);
void ctrlZhandler(int sig);

#endif