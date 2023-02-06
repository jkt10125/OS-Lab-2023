#include "./command.hpp"
extern vector<string> split(string& , char );

#include <unistd.h>
#include <fcntl.h>
using namespace std;

Command::Command(const string & __str){
    command = __str;
    parse();
    set_fd();
}
void Command::parse(){
    bool output = (command.find('>')!=string::npos);
    bool input = (command.find('<')!=string::npos);
    vector<string> tokens(3);

    if(input && output){
        int pos1 = command.find('<');
        int pos2 = command.find('>');
        tokens[0] = command.substr(0, pos1);
        tokens[1] = command.substr(pos1+1, pos2-pos1-1);
        tokens[2] = command.substr(pos2+1);
    }
    else if(input){
        int pos = command.find('<');
        tokens[0] = command.substr(0, pos);
        tokens[1] = command.substr(pos+1);
        tokens[2] = "";
    }
    else if(output){
        int pos = command.find('>');
        tokens[0] = command.substr(0, pos);
        tokens[1] = "";
        tokens[2] = command.substr(pos+1);
    }
    else{
        tokens[0] = command;
        tokens[1] = "";
        tokens[2] = "";
    }
    infile = tokens[1];
    ofile = tokens[2];
    args = split(tokens[0], ' ');
}

void Command::set_fd(){
    if(!infile.empty()){
        if((infd = open(infile.c_str(), O_RDONLY)) < 0){
            perror("open() failed");
            exit(1);
        }
        if(dup2(infd, STDIN_FILENO) < 0){
            perror("dup2() failed");
            exit(1);
        }
    }

    if(!ofile.empty()){
        if((infd = open(ofile.c_str(), O_WRONLY)) < 0){
            perror("open() failed");
            exit(1);
        }
        if(dup2(infd, STDOUT_FILENO) < 0){
            perror("dup2() failed");
            exit(1);
        }
    }
}