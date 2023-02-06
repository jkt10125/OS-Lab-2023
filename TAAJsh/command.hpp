#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <iostream>
#include <string>
#include <vector>
class Command{
    public:
        std::string command;
        std::vector<std::string> args;
        pid_t pid;
        std::string infile, ofile;
        int infd, ofd;
        
        Command(const std::string & __str);
        ~Command();
        void parse();
        void set_fd();
};
#endif