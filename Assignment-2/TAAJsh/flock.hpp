#ifndef FLOCK_HPP
#define FLOCK_HPP
#include <iostream>
#include <vector>
#define MAXLINKLEN 4096

void list_pids(const std::string &filepath, std::vector<pid_t> &pids);
void delep(std::string args);

#endif