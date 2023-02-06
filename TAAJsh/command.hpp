#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <iostream>
#include <string>
#include <vector>
class Command{
    public:
        string command;
        vector<string> args;
        pid_t pid;
        string infile, ofile;
        int infd, ofd;
        
        Command(const string & __str);
        void parse();
        void set_fd();
};
#endif