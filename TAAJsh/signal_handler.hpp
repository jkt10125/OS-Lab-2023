#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <sys/types.h>
#include "./pipeline.hpp"
#include<vector>
#include<map>
extern pid_t fgpid;

extern std::vector<Pipeline*> pipesArr;
extern std::map<pid_t, int> pid2index;

void reapProcesses(int sig);
void toggleSIGCHLDBlock(int how);
void blockSIGCHLD();
void unblockSIGCHLD();
void waitForForegroundProcess(pid_t pid);

#endif